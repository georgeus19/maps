#include "crow/crow_all.h"
#include "routing/query/setup.h"
#include "utility/point.h"
#include "routing/query/router.h"
#include "routing/query/routing_mode.h"
#include "routing/configuration_parser.h"
#include "routing/profile/profile_generator.h"
#include "routing/profile/profile.h"
#include "routing/profile/data_index.h"
#include "routing/profile/green_index.h"
#include "routing/profile/physical_length_index.h"
#include "routing/table_name_repository.h"
#include "database/database_helper.h"

#include <ostream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>

using namespace routing;
using namespace profile;
using namespace query;
using namespace database;

static std::vector<profile::Profile> GenerateProfiles(Configuration& cfg, double scale_max);
static Profile ParseProfile(const crow::json::rvalue& p);
template <typename Setup, typename Mode>
static void RunServer(Configuration& cfg, Mode& mode, const TableNameRepository& rep, const std::string& config_path);

int main(int argc, const char ** argv) {
    if (argc != 2) {
        std::cout << "Arguments: config_path" << std::endl;
    }
    std::string config_path = argv[1];
    ConfigurationParser parser{config_path};
    auto&& cfg = parser.Parse();
    
    DatabaseHelper d{cfg.database.name, cfg.database.user, cfg.database.password, cfg.database.host, cfg.database.port};
    TableNameRepository rep{cfg.algorithm->base_graph_table, cfg.algorithm->name};

    std::unordered_map<std::string, std::function<void()>> run_options{
            {
                Constants::AlgorithmNames::kDijkstra + Constants::ModeNames::kDynamicProfile,
                [&](){
                    DynamicProfileMode<DijkstraSetup> m{d, rep, Profile{100}};
                    d.DisconnectIfOpen();
                    RunServer<DijkstraSetup, DynamicProfileMode<DijkstraSetup>>(cfg, m, rep, config_path);
                }
            },
            {
                Constants::AlgorithmNames::kContractionHierarchies + Constants::ModeNames::kStaticProfile,
                [&](){
                    auto&& profiles = GenerateProfiles(cfg, 100);
                    StaticProfileMode<CHSetup> m{d, rep, profiles};
                    d.DisconnectIfOpen();
                    RunServer<CHSetup, StaticProfileMode<CHSetup>>(cfg, m, rep, config_path);
                }
            }
    };

    auto&& run_server = run_options.find(cfg.algorithm->mode);
    if (run_server == run_options.end()) {
        InvalidArgumentException{"No such mode= " + cfg.algorithm->mode + " with given algorithm " + cfg.algorithm->name + " was found"};
    }
    run_server->second();
}

static std::vector<profile::Profile> GenerateProfiles(Configuration& cfg, double scale_max) {
    DatabaseHelper d{cfg.database.name, cfg.database.user, cfg.database.password, cfg.database.host, cfg.database.port};
    ProfileGenerator gen{d, cfg.algorithm->base_graph_table, scale_max};
    for(auto&& prop : cfg.profile_properties) {
        gen.AddIndex(std::move(prop.index), std::move(prop.options));
    }
    return gen.Generate();
}

template <typename Setup>
static void GetRouters(Configuration& cfg, std::unordered_map<std::string, Router<Setup>>& routers, const TableNameRepository& rep) {
    DatabaseHelper d{cfg.database.name, cfg.database.user, cfg.database.password, cfg.database.host, cfg.database.port};
    for(auto&& profile : GenerateProfiles(cfg, 100)) {
        auto&& g = Setup::CreateGraph(d, rep.GetEdgesTable(profile));
        std::cout << " Loading " << rep.GetEdgesTable(profile) << std::endl;
        routers.emplace(profile.GetName(), Router<Setup>{std::move(g), rep.GetEdgesTable(profile)});
    }
}

static Profile ParseProfile(const crow::json::rvalue& p) {
    std::unordered_map<std::string, std::function<std::shared_ptr<DataIndex>()>> indicies{
            {Constants::IndexNames::kGreenIndex, [](){ return std::make_shared<GreenIndex>(); }},
            {Constants::IndexNames::kLengthIndex, [](){ return std::make_shared<PhysicalLengthIndex>(); }}
    };
    Profile profile{100};
    for(auto it = p.begin(); it != p.end(); ++it) {
        std::string index_name = (*it)["name"].s();
        int32_t importance = static_cast<int32_t>((*it)["importance"].i());
        auto&& index_it = indicies.find(index_name);
        if (index_it != indicies.end()) {
            profile.AddIndex(index_it->second(), importance);
        } else {
            throw InvalidArgumentException("Invalid profile query parameter - index " + index_name + " does not exist.");
        }
    }
    return profile;
}

template <typename Setup, typename Mode>
static void RunServer(Configuration& cfg, Mode& mode, const TableNameRepository& rep, const std::string& config_path) {
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
                Profile profile = ParseProfile(crow::json::load(prof));
                utility::Point source{coordinates[0]["lon"].d(), coordinates[0]["lat"].d()};
                utility::Point target{coordinates[1]["lon"].d(), coordinates[1]["lat"].d()};
                std::cout << req.url_params << std::endl;
                auto&& table_name = rep.GetEdgesTable(profile);
                auto&& router = mode.GetRouter(std::move(profile));
                std::cout << "table_name " << table_name << std::endl;
                DatabaseHelper d{cfg.database.name, cfg.database.user, cfg.database.password, cfg.database.host, cfg.database.port};
                auto&& result = router.CalculateShortestRoute(d, table_name, source, target);
                response["route"] = result; 
                response["ok"] = "true";
                
            } catch(const std::exception& e){
                std::cout << e.what() << std::endl;
                response["ok"] = "false";
            }
            return response;
    });


    CROW_ROUTE(app, "/profile_properties")([&](const crow::request& req) {
            crow::json::wvalue response;

            ConfigurationParser parser{config_path};
            auto&& cfg = parser.Parse();
            std::vector<crow::json::wvalue> result;
            for(auto&& prop : cfg.profile_properties) {
                crow::json::wvalue json_prop;
                json_prop["name"] = prop.index->GetName();
                json_prop["importance_options"] = prop.options;
                result.push_back(std::move(json_prop));
            }
            
            response["profile_properties"] = std::move(result);
            response["ok"] = "true";
            return response;
    });

    app.port(18080).multithreaded().run();
}

