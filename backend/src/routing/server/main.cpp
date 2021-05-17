#include "crow/crow_all.h"
#include <ostream>
#include <iostream>
#include "routing/query/module.h"
#include "utility/point.h"
#include "routing/query/router.h"
#include "routing/configuration_parser.h"
#include "routing/profile/profile_generator.h"
#include "database/database_helper.h"

using namespace routing;
using namespace profile;
using namespace query;
using namespace database;

int main(int argc, const char ** argv) {
    if (argc != 2) {
        std::cout << "Arguments: config_path" << std::endl;
    }
    std::string config_path = argv[1];
    crow::SimpleApp app;
    std::string edge_table = "chczedgeslength100green40";
    auto&& g = CHSetup::CreateGraph(edge_table);
    Router<CHSetup> router{g, edge_table};
    // ConfigurationParser parser{config_path};
    // auto&& cfg = parser.Parse();

    CROW_ROUTE(app, "/route")([&](const crow::request& req) {
            crow::json::wvalue response;
            std::ostringstream os;
            
            char* coor = req.url_params.get("coordinates");
            if (!coor) {
                response["error"] = "No coordinates query parameter.";
            }
            auto&& coordinates = crow::json::load(coor);
            utility::Point source{coordinates[0]["lon"].d(), coordinates[0]["lat"].d()};
            utility::Point target{coordinates[1]["lon"].d(), coordinates[1]["lat"].d()};
            std::cout << req.url_params << std::endl;
            auto&& result = router.CalculateShortestRoute("chczedgeslength100green40", source, target);
            // std::cout << result << std::endl;
            response["route"] = result; 
            response["ok"] = "true";
            return response;
    });


    CROW_ROUTE(app, "/profile_properties")([&](const crow::request& req) {
            crow::json::wvalue response;
            std::ostringstream os;

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
