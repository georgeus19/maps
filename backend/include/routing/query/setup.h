#ifndef ROUTING_QUERY_SETUP_H
#define ROUTING_QUERY_SETUP_H

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
#include "routing/query/endpoint_edges_creator.h"
#include "routing/query/endpoint_algorithm_policy.h"
#include "routing/query/edge_range_policy.h"
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
    using EndpointAlgorithmPolicy = EndpointAlgorithmPolicyContractionHierarchies<RoutingGraph<Graph>, EdgeRangePolicyVectorIterator<Edge>>;

    CHSetup(database::DatabaseHelper& d) : d_(d) {} 
    
    DbGraph CreateDbGraph() {
        return DbGraph{};
    }

    EndpointEdgesCreator<Edge> CreateEndpointEdgesCreator(database::DatabaseHelper& d, database::DbGraph* db_graph) {
        return EndpointEdgesCreator<Edge>{d, db_graph};
    }

    EndpointAlgorithmPolicy CreateEndpointAlgorithmPolicy(RoutingGraph<Graph>& routing_graph) {
        return EndpointAlgorithmPolicy{routing_graph, EdgeRangePolicyVectorIterator<Edge>{}};
    }

    static Graph CreateGraph(const std::string& graph_table_name) {
        TemporaryGraph g{};
        database::DatabaseHelper d{kDbName, kUser, kPassword, kHostAddress, kPort};
        DbGraph db_graph{};
        d.LoadFullGraph<TemporaryGraph>(graph_table_name, g, &db_graph);
        d.LoadAdditionalVertexProperties(graph_table_name + "_vertices", g);
        Graph search_graph{};
        search_graph.Load(g);
        return search_graph;
    }

private: 
    database::DatabaseHelper& d_;
};



}
}

#endif //ROUTING_QUERY_SETUP_H