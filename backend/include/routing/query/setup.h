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
#include "routing/bidirectional_graph.h"
#include "routing/query/bidirectional_dijkstra.h"
#include "routing/query/endpoint_edges_creator.h"
#include "routing/query/endpoint_algorithm_policy.h"
#include "routing/query/edge_range_policy.h"
#include "routing/edges/ch_edge.h"
#include "routing/edge_ranges/vector_edge_range.h"
#include "routing/vertices/ch_vertex.h"
#include "routing/ch_search_graph.h"
#include "routing/routing_graph.h"
#include "routing/edge_ranges/iterator_edge_range.h"
#include "routing/edges/length_source.h"
#include "routing/edge_factory.h"

#include "routing/table_name_repository.h"

#include <string>

namespace routing {
namespace query {

class DijkstraSetup {
public:
    using Edge = BasicEdge<NumberLengthSource>;
    using EdgeRange = VectorEdgeRange<Edge>;
    using Vertex = BasicVertex<Edge, EdgeRange>;
    using DbGraph = database::UnpreprocessedDbGraph;

    using Graph = AdjacencyListGraph<Vertex, Edge>;
    using Algorithm = Dijkstra<RoutingGraph<Graph>>;
    using EndpointAlgorithmPolicy = EndpointAlgorithmPolicyDijkstra<RoutingGraph<Graph>, EdgeRangePolicyVector<Edge>>;

    static Graph CreateGraph(database::DatabaseHelper& d, const std::string& graph_table_name) {
        Graph g{};
        DbGraph db_graph{};
        BasicNumberEdgeFactory edge_factory{};
        d.LoadGraphEdges<Graph>(graph_table_name, g, &db_graph, edge_factory);
        return g;
    }

    EndpointEdgesCreator<Edge> CreateEndpointEdgesCreator(database::DatabaseHelper& d, database::DbGraph* db_graph) {
        return EndpointEdgesCreator<Edge>{d, db_graph};
    }

    EndpointAlgorithmPolicy CreateEndpointAlgorithmPolicy(RoutingGraph<Graph>& routing_graph) {
        return EndpointAlgorithmPolicy{routing_graph, EdgeRangePolicyVector<Edge>{}};
    }

    DbGraph CreateDbGraph() {
        return DbGraph{};
    }
};

class CHSetup {
public:
    using Edge = CHEdge<NumberLengthSource>;
    using EdgeIterator = std::vector<Edge>::iterator;
    using EdgeRange = IteratorEdgeRange<Edge, EdgeIterator>;
    using Vertex = CHVertex<Edge, EdgeRange>;
    
    using TemporaryGraph = BidirectionalGraph<AdjacencyListGraph<CHVertex<Edge, VectorEdgeRange<Edge>>, Edge>>;
    using Graph = CHSearchGraph<Vertex, Edge>;
    using DbGraph = database::CHDbGraph;
    using Algorithm = BidirectionalDijkstra<RoutingGraph<Graph>>;
    using EndpointAlgorithmPolicy = EndpointAlgorithmPolicyContractionHierarchies<RoutingGraph<Graph>, EdgeRangePolicyVectorIterator<Edge>>;

    static Graph CreateGraph(database::DatabaseHelper& d, const std::string& graph_table_name) {
        TemporaryGraph g{};
        DbGraph db_graph{};
        CHNumberEdgeFactory edge_factory{};
        d.LoadGraphEdges<TemporaryGraph>(graph_table_name, g, &db_graph, edge_factory);
        d.LoadAdditionalVertexProperties(graph_table_name + "_vertices", g);
        Graph search_graph{};
        search_graph.Load(g);
        return search_graph;
    }

    DbGraph CreateDbGraph() {
        return DbGraph{};
    }

    EndpointEdgesCreator<Edge> CreateEndpointEdgesCreator(database::DatabaseHelper& d, database::DbGraph* db_graph) {
        return EndpointEdgesCreator<Edge>{d, db_graph};
    }

    EndpointAlgorithmPolicy CreateEndpointAlgorithmPolicy(RoutingGraph<Graph>& routing_graph) {
        return EndpointAlgorithmPolicy{routing_graph, EdgeRangePolicyVectorIterator<Edge>{}};
    }
};



}
}
#endif //ROUTING_QUERY_SETUP_H