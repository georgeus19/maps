#ifndef BACKEND_QUERY_MAIN_H
#define BACKEND_QUERY_MAIN_H

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

#include <string>

namespace routing {
namespace query {

/**
 * Info to connect to the database with a routing graph.
 */
const std::string kDbName = "gis";
const std::string kUser = "postgres";
const std::string kPassword = "wtz2trln";
const std::string kHostAddress = "127.0.0.1";
const std::string kPort = "5432";

struct DijkstraSetup {
    using Graph = AdjacencyListGraph<BasicVertex<BasicEdge>, BasicEdge>;
    using DbGraph = database::UnpreprocessedDbGraph;
    using Algorithm = Dijkstra<Graph>;
};

struct CHSetup {
    using Graph = BidirectionalGraph<AdjacencyListGraph<ContractionSearchVertex<CHSearchEdge>, CHSearchEdge>>;
    using DbGraph = database::CHDbGraph;
    using Algorithm = BidirectionalDijkstra<Graph>;
};
/**
 * Calculate shortest path between `start` and `end` points.
 *
 * @param table_name Table where routing graph is.
 * @param start Route start.
 * @param end Route end.
 * @return geoJson list of geometries of edges that are part of the shortest route all in string.
 */
template <typename Setup>
std::string CCalculateShortestRoute(const std::string & table_name, utility::Point start, utility::Point end) {
    if (start.lat_ == end.lat_ && start.lon_ == end.lon_) {
        throw RouteNotFoundException("Start and end point are the same.");
    }
    // Load graph.
    utility::Point graph_center{(start.lon_ + end.lon_) / 2, (start.lat_ + end.lat_) / 2};
    typename Setup::Graph g{};
    database::DatabaseHelper d{kDbName, kUser, kPassword, kHostAddress, kPort};
    std::string radius = d.CalculateRadius(start, end, 0.7);
    typename Setup::DbGraph db_graph{};
    d.LoadGraph<typename Setup::Graph>(graph_center, radius, table_name, g, &db_graph);

    // Add start segments to graph.
    EndpointHandler<BasicEdgeEndpointHandler> start_handler{1, 1, 0, 0};
    auto && start_edges = start_handler.CalculateEndpointEdges(start, table_name, d);
    for(auto&& edge : start_edges) {
        auto e = edge;
        g.AddEdge(std::move(e));
    }

    // Add end segments to graph.
    unsigned_id_type free_node_id_from = start_handler.get_node_id_to();
    unsigned_id_type free_edge_id_from = start_handler.get_edge_id_to();
    EndpointHandler<BasicEdgeEndpointHandler> end_handler{free_node_id_from, free_node_id_from + 1, free_edge_id_from, free_edge_id_from + 4};
    auto && end_edges = end_handler.CalculateEndpointEdges(end, table_name, d);
    for(auto&& edge : end_edges) {
        auto e = edge;
        g.AddEdge(std::move(e));
    }

    // Run routing algorithm.
    Algorithm<typename Setup::Algorithm> alg{g};
    alg.Run(0, 1);
    std::vector<typename Setup::Algorithm::Edge> res = alg.GetRoute();
    // Construct list of geometries.
    std::string geojson_array = d.GetRouteCoordinates(res, table_name);

    std::string first_edge_geometry = start_handler.GetEndpointEdgeGeometry(res[res.size() - 1].get_uid());
    std::string last_edge_geometry = end_handler.GetEndpointEdgeGeometry(res[0].get_uid());

    std::string final_array = "[" + first_edge_geometry + geojson_array + "," + last_edge_geometry + "]";
    return final_array;
}




}
}

#endif //BACKEND_QUERY_MAIN_H
