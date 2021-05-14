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
    crow::SimpleApp app;
    std::string edge_table = "chczedgeslength100green0";
    auto&& g = CHSetup::CreateGraph(edge_table);
    Router<CHSetup> router{g, edge_table};
    // ConfigurationParser parser{argv[1]};
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
            auto&& result = router.CalculateShortestRoute("chczedgeslength100green0", source, target);
            // auto&& result = CCalculateShortestRoute<DijkstraSetup>("czedges", source, target);
            // std::cout << result << std::endl;
            response["route"] = result; //"[{\"lon\":13.395043407996823,\"lat\":49.731248062403814},{\"lon\":13.399688415374477,\"lat\":49.72567213250674}]";
            response["ok"] = "true";
            return response;
    });

    app.port(18080).multithreaded().run();
}
