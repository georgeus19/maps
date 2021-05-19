#ifndef BACKEND_QUERY_ROUTER_H
#define BACKEND_QUERY_ROUTER_H

#include "routing/routing_graph.h"
#include "routing/adjacency_list_graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/algorithm.h"
#include "routing/query/dijkstra.h"
#include "routing/exception.h"
#include "database/database_helper.h"
#include "database/db_graph.h"
#include "database/db_edge_iterator.h"
#include "utility/point.h"
#include "routing/endpoint_handler.h"
#include "routing/bidirectional_graph.h"
#include "routing/query/bidirectional_dijkstra.h"
#include "routing/basic_edge_endpoint_handler.h"
#include "routing/edges/ch_search_edge.h"
#include "routing/vertices/contraction_search_vertex.h"
#include "routing/edge_ranges/vector_edge_range.h"
#include "routing/vertices/ch_vertex.h"
#include "routing/ch_search_graph.h"
#include "routing/routing_graph.h"
#include "routing/query/module.h"

#include <string>

namespace routing {
namespace query {

template <typename Setup>
class Router {
public:
    Router(typename Setup::Graph&& graph, const std::string& graph_table_name) 
        : base_graph_(std::move(graph)), base_graph_max_vertex_id_(base_graph_.GetMaxVertexId()), base_graph_max_edge_id_(base_graph_.GetMaxEdgeId()) {}

    std::string CalculateShortestRoute(const std::string& table_name, utility::Point source, utility::Point target) {
        if (source.lat_ == target.lat_ && source.lon_ == target.lon_) {
            throw RouteNotFoundException("Start and end point are the same.");
        }
        database::DatabaseHelper d{kDbName, kUser, kPassword, kHostAddress, kPort};
        Setup setup{d, table_name};
        RoutingGraph<typename Setup::Graph> routing_graph{base_graph_};
        auto&& db_graph = setup.CreateDbGraph();
        auto&& route_endpoints = setup.AddRouteEndpoints(routing_graph, source, target, base_graph_max_vertex_id_ + 1, base_graph_max_edge_id_ + 1);

            auto start_run = std::chrono::high_resolution_clock::now();
        Algorithm<typename Setup::Algorithm> alg{routing_graph};
        alg.Run(route_endpoints.source, route_endpoints.target);
            auto finish_run = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed_run = finish_run - start_run;
            std::cout << "Elapsed time - run alg: " << elapsed_run.count() << " s\n";

        std::vector<typename Setup::Algorithm::Edge> res = alg.GetRoute();
        std::cout << "Get route done." << std::endl;
        return GetRouteGeometry(d, setup, res);
    }


    std::string GetRouteGeometry(database::DatabaseHelper& d, Setup& setup, std::vector<typename Setup::Algorithm::Edge>& route) {
        std::string first_edge_geometry = setup.get_source_endpoint_handler().GetEndpointEdgeGeometry(route[0].get_uid());
        std::string last_edge_geometry = setup.get_target_endpoint_handler().GetEndpointEdgeGeometry(route[route.size() - 1].get_uid());
        route.pop_back();
        route.erase(route.begin());
        std::string geojson_array = d.GetRouteCoordinates(route, setup.get_edges_table_name());
        std::cout << "GetRouteCoordinates" << std::endl;

        std::string final_array = "[" + first_edge_geometry + "," + geojson_array + last_edge_geometry + "]";
        std::cout << "Routing done." << std::endl;
        return final_array;
    }


private:
    typename Setup::Graph base_graph_;
    unsigned_id_type base_graph_max_vertex_id_;
    unsigned_id_type base_graph_max_edge_id_;
};


}
}

#endif //BACKEND_QUERY_ROUTER_H
