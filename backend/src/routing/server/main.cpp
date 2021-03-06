#include "crow/crow_all.h"
#include "routing/query/algorithm_factory.h"
#include "routing/utility/point.h"
#include "routing/query/router.h"
#include "routing/query/routing_mode.h"
#include "routing/configuration_parser.h"
#include "routing/profile/profile_generator.h"
#include "routing/profile/profile.h"
#include "routing/profile/preference_index.h"
#include "routing/profile/green_index.h"
#include "routing/profile/physical_length_index.h"
#include "routing/table_names.h"
#include "routing/database/database_helper.h"
#include "routing/types.h"

#include <ostream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <chrono>
#include <functional>

using namespace routing;
using namespace profile;
using namespace query;
using namespace database;

/**
 * Parse profile that was received as a part of a routing request.
 */
static Profile ParseProfile(const crow::json::rvalue& p, Profile& default_profile);

/**
 * Run routing server with the selected Profile mode. This methods never returns.
 */
template <typename Setup, typename Mode>
static void RunServer(Configuration& cfg, Mode& mode, const std::string& config_path);

int main(int argc, const char ** argv) {
    if (argc != 2) {
        std::cout << "Arguments: config_path" << std::endl;
        return 1;
    }
    std::string config_path = argv[1];
    ConfigurationParser parser{config_path};
    auto&& cfg = parser.Parse();
    DatabaseHelper d{cfg.database.name, cfg.database.user, cfg.database.password, cfg.database.host, cfg.database.port};
    

    std::unordered_map<std::string, std::function<void()>> run_options{
            {
                Constants::AlgorithmNames::kDijkstra + Constants::ModeNames::kDynamicProfile,
                [&](){
                    cfg.profile_preferences.LoadIndices(d);
                    auto&& gen = cfg.profile_preferences.GetProfileGenerator();
                    Profile profile = gen.GetFrontProfile();
                    DynamicProfileMode<DijkstraFactory> m{d, std::make_unique<DijkstraTableNames>(cfg.algorithm->base_graph_table), std::move(profile)};
                    std::cout << Constants::AlgorithmNames::kDijkstra + Constants::ModeNames::kDynamicProfile << " run mode" << std::endl;
                    d.DisconnectIfOpen();
                    RunServer<DijkstraFactory, DynamicProfileMode<DijkstraFactory>>(cfg, m, config_path);
                }
            },
            {
                Constants::AlgorithmNames::kContractionHierarchies + Constants::ModeNames::kStaticProfile,
                [&](){
                    auto&& gen = cfg.profile_preferences.GetProfileGenerator();
                    std::cout << Constants::AlgorithmNames::kContractionHierarchies + Constants::ModeNames::kStaticProfile << " run mode" << std::endl;
                    StaticProfileMode<CHStaticFactory> m{};
                    cfg.profile_preferences.base_index->Load(d, cfg.profile_preferences.base_index_table);
                    for(auto&& profile : gen.Generate()) {
                        m.AddRouter(d, std::make_unique<CHTableNames>(cfg.algorithm->base_graph_table, profile), profile);
                    }
                    d.DisconnectIfOpen();
                    RunServer<CHStaticFactory, StaticProfileMode<CHStaticFactory>>(cfg, m, config_path);
                }
            },
            {
                Constants::AlgorithmNames::kContractionHierarchies + Constants::ModeNames::kDynamicProfile,
                [&](){
                    auto&& gen = cfg.profile_preferences.GetProfileGenerator();
                    Profile profile = gen.GetFrontProfile();
                    std::unique_ptr<TableNames> table_names = std::make_unique<CHTableNames>(cfg.algorithm->base_graph_table, profile);
                    cfg.profile_preferences.LoadIndices(d, table_names->GetIndexTablePrefix());
                    std::cout << Constants::AlgorithmNames::kContractionHierarchies + Constants::ModeNames::kDynamicProfile << " run mode" << std::endl;
                    DynamicProfileMode<CHDynamicFactory> m{d, std::move(table_names), std::move(profile)};
                    d.DisconnectIfOpen();
                    RunServer<CHDynamicFactory, DynamicProfileMode<CHDynamicFactory>>(cfg, m, config_path);
                }
            }
    };

    auto run_server = run_options.find(cfg.algorithm->name + cfg.algorithm->mode);
    if (run_server != run_options.end()) {
        std::cout << cfg.algorithm->mode << std::endl;
        run_server->second();
    } else {
        throw InvalidArgumentException{"No such mode= " + cfg.algorithm->mode + " with given algorithm " + cfg.algorithm->name + " was found"};
    }
}

static Profile ParseProfile(const crow::json::rvalue& p, Profile& default_profile) {
    Profile profile{default_profile.GetBaseIndex()};
    // std::cout << "default_profile " << default_profile.GetName() << std::endl; 
    for(auto it = p.begin(); it != p.end(); ++it) {
        std::string index_name = (*it)["name"].s();
        float importance = static_cast<float>((*it)["importance"].d());
        profile.AddIndex(default_profile.GetIndex(index_name), importance);
    }
    return profile;
}


template <typename Setup, typename Mode>
static void RunServer(Configuration& cfg, Mode& mode, const std::string& config_path) {
    crow::SimpleApp app;
    CROW_ROUTE(app, "/route")([&](const crow::request& req) {
            crow::json::wvalue response;
            try {
                char* coor = req.url_params.get("coordinates");
                if (!coor) {
                    response["error"] = "No coordinates query parameter.";
                }
                char* prof = req.url_params.get("profile");
                if (!prof) {
                    response["error"] = "No profile query parameter.";
                }
                auto&& coordinates = crow::json::load(coor);
                Profile profile = ParseProfile(crow::json::load(prof), mode.GetDefaultProfile());
                utility::Point source{static_cast<float>(coordinates[0]["lon"].d()), static_cast<float>(coordinates[0]["lat"].d())};
                utility::Point target{static_cast<float>(coordinates[1]["lon"].d()), static_cast<float>(coordinates[1]["lat"].d())};
                std::cout << req.url_params << std::endl;
                auto&& router = mode.GetRouter(std::move(profile));
                DatabaseHelper d{cfg.database.name, cfg.database.user, cfg.database.password, cfg.database.host, cfg.database.port};
                auto&& route = router.CalculateShortestRoute(d, source, target);
                response["route"] = route.get_geometry();
                response["length"] = route.GetLength(mode.GetDefaultProfile().GetBaseIndex().get());
                response["ok"] = "true";
                
            } catch(const std::exception& e){
                std::cout << e.what() << std::endl;
                response["ok"] = "false";
            }
            return response;
    });


    CROW_ROUTE(app, "/profile_preferences")([&](const crow::request& req) {
            crow::json::wvalue response;

            ConfigurationParser parser{config_path};
            auto&& cfg = parser.Parse();
            std::vector<crow::json::wvalue> result;
            for(auto&& prop : cfg.profile_preferences.properties) {
                crow::json::wvalue json_prop;
                json_prop["name"] = prop.index->GetName();
                json_prop["importance_options"] = prop.options;
                json_prop["display_name"] = prop.display_name;
                json_prop["display_importance_options"] = prop.option_display_names;
                result.push_back(std::move(json_prop));
            }
            
            response["profile_preferences"] = std::move(result);
            response["ok"] = "true";
            return response;
    });

    // In dynamic profile, the profile of the road graph is changed; threfore,
    // only singlethreaded server is possible.
    if (cfg.algorithm->mode == Constants::ModeNames::kDynamicProfile) {
        app.port(18080).concurrency(1).run();
    } else {
        app.port(18080).multithreaded().run();
    }
}

