#include "crow/crow_all.h"
#include "routing/query/setup.h"
#include "utility/point.h"
#include "routing/query/router.h"
#include "routing/configuration_parser.h"
#include "routing/profile/profile_generator.h"
#include "routing/table_name_repository.h"
#include "database/database_helper.h"

#include <ostream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

using namespace routing;
using namespace profile;
using namespace query;
using namespace database;

static std::vector<profile::Profile> GenerateProfiles(Configuration& cfg, double scale_max) {
    DatabaseHelper d{cfg.database.name, cfg.database.user, cfg.database.password, cfg.database.host, cfg.database.port};
    ProfileGenerator gen{d, cfg.algorithm->base_graph_table, scale_max};
    for(auto&& prop : cfg.profile_properties) {
        gen.AddIndex(std::move(prop.index), std::move(prop.options));
    }
    return gen.Generate();
}

int main(int argc, const char ** argv) {
    if (argc != 2) {
        std::cout << "Arguments: config_path" << std::endl;
    }
    std::string config_path = argv[1];
    crow::SimpleApp app;
    ConfigurationParser parser{config_path};
    auto&& cfg = parser.Parse();
    using Setup = CHSetup;
    std::unordered_set<std::string> index_names{};
    for(auto&& prop : cfg.profile_properties) {
        index_names.insert(prop.index->GetName());
    }
    std::unordered_map<std::string, Router<Setup>> routers{};
    TableNameRepository rep{cfg.algorithm->base_graph_table, cfg.algorithm->name};
    for(auto&& profile : GenerateProfiles(cfg, 100)) {
        auto&& g = Setup::CreateGraph(rep.GetEdgesTable(profile));
        std::cout << " Loading " << rep.GetEdgesTable(profile) << std::endl;
        routers.emplace(profile.GetName(), Router<Setup>{std::move(g), rep.GetEdgesTable(profile)});
    }

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
                auto&& profile = crow::json::load(prof);
                std::string profile_name;
                for(auto it = profile.begin(); it != profile.end(); ++it) {
                    std::string index_name = (*it)["name"].s();
                    if (!index_names.contains(index_name)) {
                        throw InvalidArgumentException("Invalid profile query parameter - index " + index_name + " does not exist.");
                    }
                    profile_name += index_name;
                    profile_name += std::to_string((*it)["importance"].i());
                }
                utility::Point source{coordinates[0]["lon"].d(), coordinates[0]["lat"].d()};
                utility::Point target{coordinates[1]["lon"].d(), coordinates[1]["lat"].d()};
                std::cout << req.url_params << std::endl;
                auto&& table_name = rep.GetEdgesTable(profile_name);
                auto&& it = routers.find(profile_name);
                if (it != routers.end()) {
                    std::cout << "table_name " << table_name << std::endl;
                    auto&& result = it->second.CalculateShortestRoute(table_name, source, target);
                    response["route"] = result; 
                    response["ok"] = "true";
                } else {
                    throw InvalidArgumentException{"Invalid profile query parameter - no table " + table_name + " exists."};
                }
                
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
