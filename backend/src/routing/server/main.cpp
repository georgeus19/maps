#include "crow/crow_all.h"
#include <ostream>
#include <iostream>
#include "routing/query/module.h"
#include "utility/point.h"

using namespace routing;
using namespace query;


int main(int argc, const char ** argv) {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/route")([](const crow::request& req) {
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
            auto&& result = CCalculateShortestRoute<DijkstraSetup>("cznoloops", source, target);
            std::cout << result << std::endl;
            response["route"] = result; //"[{\"lon\":13.395043407996823,\"lat\":49.731248062403814},{\"lon\":13.399688415374477,\"lat\":49.72567213250674}]";
            response["ok"] = "true";
            return response;
    });

    app.port(18080).multithreaded().run();
}
