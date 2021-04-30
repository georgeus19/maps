#ifndef BACKEND_QUERY_MAIN_H
#define BACKEND_QUERY_MAIN_H

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
#include "routing/edge_ranges/iterator_edge_range.h"

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

// class DijkstraSetup {
// public:
//     using Edge = BasicEdge;
//     using Vertex = BasicVertex<Edge, VectorEdgeRange<Edge>>;
//     using Graph = AdjacencyListGraph<Vertex, Edge>;
//     using DbGraph = database::UnpreprocessedDbGraph;
//     using Algorithm = Dijkstra<Graph>;

//     DijkstraSetup(database::DatabaseHelper& d) : d_(d) {} 

//     Graph CreateGraph(const std::string& graph_table_name) {
//         Graph g{};
//         DbGraph db_graph{};
//         d_.LoadFullGraph<Graph>(graph_table_name, g, &db_graph);
//         return g;
//     }

//     DbGraph CreateDbGraph() {
//         return DbGraph{};
//     }

//     BasicEdgeEndpointHandler<Edge>& get_source_endpoint_handler() {
//         return source_endpoint_handler_;
//     }

//     BasicEdgeEndpointHandler<Edge>& get_target_endpoint_handler() {
//         return target_endpoint_handler_;
//     }

// private:
//     database::DatabaseHelper& d_;
//     BasicEdgeEndpointHandler<Edge> source_endpoint_handler_;
//     BasicEdgeEndpointHandler<Edge> target_endpoint_handler_;
// };

class CHSetup {
public:
    using Edge = CHSearchEdge;
    using EdgeIterator = std::vector<Edge>::iterator;
    using EdgeRange = IteratorEdgeRange<Edge, EdgeIterator>;
    using Vertex = CHVertex<Edge, EdgeRange>;
    
    using TemporaryGraph = BidirectionalGraph<AdjacencyListGraph<CHVertex<Edge, VectorEdgeRange<Edge>>, Edge>>;
    using Graph = CHSearchGraph<Vertex, Edge>;
    using DbGraph = database::CHDbGraph;
    using Algorithm = BidirectionalDijkstra<RoutingGraph<Graph>>;

    struct RouteEndpoints {
        RouteEndpoints(unsigned_id_type s, unsigned_id_type t) : source(s), target(t) {}
        unsigned_id_type source;
        unsigned_id_type target;
    };

    CHSetup(database::DatabaseHelper& d, const std::string& edges_table_name) : d_(d), edges_table_name_(edges_table_name) {
        edges_.reserve(10);
    } 
    
    DbGraph CreateDbGraph() {
        return DbGraph{};
    }

    static Graph CreateGraph(const std::string& graph_table_name) {
        TemporaryGraph g{};
        database::DatabaseHelper d{kDbName, kUser, kPassword, kHostAddress, kPort};
        DbGraph db_graph{};
        d.LoadFullGraph<TemporaryGraph>(graph_table_name, g, &db_graph);
        d.LoadAdditionalVertexProperties(graph_table_name + "_vertex_ordering", g);
        Graph search_graph{};
        search_graph.Load(g);
        return search_graph;
    }

    RouteEndpoints AddRouteEndpoints(RoutingGraph<Graph>& routing_graph, utility::Point source, utility::Point target,
        unsigned_id_type free_vertex_id, unsigned_id_type free_edge_id) {
        unsigned_id_type source_vertex_id = free_vertex_id;
        source_endpoint_handler_ = BasicEdgeEndpointHandler<Edge>{&db_graph_, source_vertex_id, free_edge_id};
        auto&& start_edges = source_endpoint_handler_.CalculateEndpointEdges(source, edges_table_name_, d_);
        auto source_end_it = edges_.begin();
        for(auto&& edge : start_edges) {
            if (source_vertex_id == edge.get_from()) {
                edges_.push_back(edge);
                ++source_end_it;
            }
        }
        Vertex source_vertex{source_vertex_id, EdgeRange{edges_.begin(), source_end_it}, 0}; 
        routing_graph.AddVertex(std::move(source_vertex));
        unsigned_id_type target_vertex_id = source_vertex_id + 1;
        free_edge_id += start_edges.size();
        target_endpoint_handler_ = BasicEdgeEndpointHandler<Edge>{&db_graph_, target_vertex_id, free_edge_id};
        auto&& target_edges = target_endpoint_handler_.CalculateEndpointEdges(target, edges_table_name_, d_);
        for(auto&& edge : target_edges) {
            if (target_vertex_id == edge.get_from()) {
                edge.SetBackward();
                edges_.push_back(edge);
            }
        }
        std::cout << "edges_.size() = " << edges_.size() << std::endl;
        Vertex target_vertex{target_vertex_id, EdgeRange{source_end_it, edges_.end()}, 0}; 
        routing_graph.AddVertex(std::move(target_vertex));
        return RouteEndpoints{source_vertex_id, target_vertex_id};
    }

    BasicEdgeEndpointHandler<Edge>& get_source_endpoint_handler() {
        return source_endpoint_handler_;
    }

    BasicEdgeEndpointHandler<Edge>& get_target_endpoint_handler() {
        return target_endpoint_handler_;
    }

    const std::string& get_edges_table_name() {
        return edges_table_name_;
    }

private: 
    database::DatabaseHelper& d_;
    DbGraph db_graph_;
    std::string edges_table_name_;
    std::vector<Edge> edges_;
    BasicEdgeEndpointHandler<Edge> source_endpoint_handler_;
    BasicEdgeEndpointHandler<Edge> target_endpoint_handler_;
};



}
}

#endif //BACKEND_QUERY_MAIN_H
