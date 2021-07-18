#ifndef ROUTING_QUERY_ROUTER_H
#define ROUTING_QUERY_ROUTER_H

#include "routing/routing_graph.h"
#include "routing/adjacency_list_graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/algorithm.h"
#include "routing/query/dijkstra.h"
#include "routing/exception.h"
#include "routing/database/database_helper.h"
#include "routing/utility/point.h"
#include "routing/bidirectional_graph.h"
#include "routing/query/bidirectional_dijkstra.h"
#include "routing/edges/ch_edge.h"
#include "routing/table_names.h"
#include "routing/edge_ranges/vector_edge_range.h"
#include "routing/vertices/ch_vertex.h"
#include "routing/ch_search_graph.h"
#include "routing/routing_graph.h"
#include "routing/query/endpoint_edges_creator.h"
#include "routing/query/endpoints_creator.h"
#include "routing/query/route.h"
#include "routing/types.h"

#include "routing/database/db_graph.h"
#include "routing/database/db_edge_iterator.h"

#include <string>
#include <memory>
#include <utility>
#include <vector>
#include <chrono>

namespace routing {
namespace query {

template <typename AlgorithmFactory>
class Router {
    using EC = EndpointsCreator<typename AlgorithmFactory::EndpointAlgorithmPolicy, EndpointEdgesCreator<typename AlgorithmFactory::EndpointEdgeFactory>>;
public:
    Router() : alg_factory_(), base_graph_(), table_names_(), base_graph_max_vertex_id_(), base_graph_max_edge_id_() {}

    Router(const AlgorithmFactory& af, typename AlgorithmFactory::Graph&& graph, std::unique_ptr<TableNames>&& table_names) 
        : alg_factory_(af), base_graph_(std::move(graph)), table_names_(std::move(table_names)),
            base_graph_max_vertex_id_(base_graph_.GetMaxVertexId()), base_graph_max_edge_id_(base_graph_.GetMaxEdgeId()) {}

    Router(Router&& other)= default;
    Router(const Router& other) = delete;
    Router& operator=(Router&& other) = default;
    Router operator=(const Router& other) = delete;
    ~Router() = default;

    Route<typename AlgorithmFactory::Algorithm::Edge> CalculateShortestRoute(database::DatabaseHelper& d, utility::Point source, utility::Point target) {
        if (source.lat_ == target.lat_ && source.lon_ == target.lon_) {
            throw RouteNotFoundException("Start and end point are the same.");
        }
        
        RoutingGraph<typename AlgorithmFactory::Graph> routing_graph{base_graph_};
        auto&& db_graph = alg_factory_.CreateDbGraph();

        EC endpoints_creator{
            alg_factory_.CreateEndpointAlgorithmPolicy(routing_graph),
            alg_factory_.CreateEndpointEdgesCreator(d, &db_graph)
        };
        unsigned_id_type source_vertex_id = base_graph_max_vertex_id_ + 1;
        unsigned_id_type target_vertex_id = base_graph_max_vertex_id_ + 2;
        endpoints_creator.AddSourceEndpoint(table_names_->GetEdgesTable(), source_vertex_id, source);
        endpoints_creator.AddTargetEndpoint(table_names_->GetEdgesTable(), target_vertex_id, target);

        Algorithm<typename AlgorithmFactory::Algorithm> alg{routing_graph};
        alg.Run(source_vertex_id, target_vertex_id);            

        std::vector<typename AlgorithmFactory::Algorithm::Edge> route = alg.GetRoute();
        auto&& geom = GetRouteGeometry(d, endpoints_creator, route);
        return Route<typename AlgorithmFactory::Algorithm::Edge>{std::move(route), std::move(geom)};
    }

    std::string GetRouteGeometry(database::DatabaseHelper& d, EC& endpoints_creator,
        const std::vector<typename AlgorithmFactory::Algorithm::Edge>& route) {
        auto&& route_begin = route.cbegin();
        auto&& route_end = route.cend();
        --route_end;
        auto&& first_edge_geometry = endpoints_creator.GetSourceGeometry(route_begin->get_uid());
        auto&& last_edge_geometry = endpoints_creator.GetTargetGeometry(route_end->get_uid());
        ++route_begin;
        // The first and the last edge geometries already retrieved so
        // inc route_begin and dec route_end was done.
        auto&& geojson_array = d.GetRouteCoordinates<typename AlgorithmFactory::Algorithm::Edge>(route_begin, route_end, table_names_->GetEdgesTable());

        std::string final_array = "[" + first_edge_geometry + "," + geojson_array + last_edge_geometry + "]";
        return final_array;
    }

private:
    AlgorithmFactory alg_factory_;
    typename AlgorithmFactory::Graph base_graph_;
    std::unique_ptr<TableNames> table_names_;
    unsigned_id_type base_graph_max_vertex_id_;
    unsigned_id_type base_graph_max_edge_id_;
};




}
}
#endif //ROUTING_QUERY_ROUTER_H