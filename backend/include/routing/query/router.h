#ifndef ROUTING_QUERY_ROUTER_H
#define ROUTING_QUERY_ROUTER_H

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
#include "routing/bidirectional_graph.h"
#include "routing/query/bidirectional_dijkstra.h"
#include "routing/edges/ch_edge.h"
#include "routing/table_names.h"
#include "routing/edge_ranges/vector_edge_range.h"
#include "routing/vertices/ch_vertex.h"
#include "routing/ch_search_graph.h"
#include "routing/routing_graph.h"
#include "routing/query/setup.h"
#include "routing/query/endpoints_creator.h"

#include <string>
#include <memory>

namespace routing {
namespace query {

template <typename Setup>
class Router {
    using EC = EndpointsCreator<typename Setup::EndpointAlgorithmPolicy, EndpointEdgesCreator<typename Setup::EndpointEdgeFactory>>;
public:
    Router() : setup_(), base_graph_(), table_names_(), base_graph_max_vertex_id_(), base_graph_max_edge_id_() {}

    Router(const Setup& setup, typename Setup::Graph&& graph, std::unique_ptr<TableNames>&& table_names) 
        : setup_(setup), base_graph_(std::move(graph)), table_names_(std::move(table_names)),
            base_graph_max_vertex_id_(base_graph_.GetMaxVertexId()), base_graph_max_edge_id_(base_graph_.GetMaxEdgeId()) {}

    Router(Router&& other)= default;
    Router(const Router& other) = delete;
    Router& operator=(Router&& other) = default;
    Router operator=(const Router& other) = delete;
    ~Router() = default;

    std::string CalculateShortestRoute(database::DatabaseHelper& d, utility::Point source, utility::Point target) {
        std::cout << "Routing on " << table_names_->GetEdgesTable() << std::endl;
        if (source.lat_ == target.lat_ && source.lon_ == target.lon_) {
            throw RouteNotFoundException("Start and end point are the same.");
        }
        RoutingGraph<typename Setup::Graph> routing_graph{base_graph_};
        auto&& db_graph = setup_.CreateDbGraph();

        EC endpoints_creator{
            setup_.CreateEndpointAlgorithmPolicy(routing_graph),
            setup_.CreateEndpointEdgesCreator(d, &db_graph)
        };
        unsigned_id_type source_vertex_id = base_graph_max_vertex_id_ + 1;
        unsigned_id_type target_vertex_id = base_graph_max_vertex_id_ + 2;
        endpoints_creator.AddSourceEndpoint(table_names_->GetEdgesTable(), source_vertex_id, source);
        endpoints_creator.AddTargetEndpoint(table_names_->GetEdgesTable(), target_vertex_id, target);


            auto start_run = std::chrono::high_resolution_clock::now();
        Algorithm<typename Setup::Algorithm> alg{routing_graph};
        alg.Run(source_vertex_id, target_vertex_id);
            auto finish_run = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed_run = finish_run - start_run;
            std::cout << "Elapsed time - run alg: " << elapsed_run.count() << " s\n";

        std::vector<typename Setup::Algorithm::Edge> res = alg.GetRoute();
        std::cout << "Get route done." << std::endl;
        return GetRouteGeometry(d, endpoints_creator, res);
    }

    std::string GetRouteGeometry(database::DatabaseHelper& d, EC& endpoints_creator,
        std::vector<typename Setup::Algorithm::Edge>& route) {
        auto&& first_edge_geometry = endpoints_creator.GetSourceGeometry(route[0].get_uid());
        auto&& last_edge_geometry = endpoints_creator.GetTargetGeometry(route[route.size() - 1].get_uid());
        route.pop_back();
        route.erase(route.begin());
        auto&& geojson_array = d.GetRouteCoordinates(route, table_names_->GetEdgesTable());

        std::string final_array = "[" + first_edge_geometry + "," + geojson_array + last_edge_geometry + "]";
        std::cout << "Routing done." << std::endl;
        return final_array;
    }

private:
    Setup setup_;
    typename Setup::Graph base_graph_;
    std::unique_ptr<TableNames> table_names_;
    unsigned_id_type base_graph_max_vertex_id_;
    unsigned_id_type base_graph_max_edge_id_;
};




}
}
#endif //ROUTING_QUERY_ROUTER_H