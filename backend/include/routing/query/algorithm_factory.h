#ifndef ROUTING_QUERY_ALGORITHM_FACTORY_H
#define ROUTING_QUERY_ALGORITHM_FACTORY_H

#include "routing/routing_graph.h"
#include "routing/adjacency_list_graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/algorithm.h"
#include "routing/query/dijkstra.h"
#include "routing/exception.h"
#include "routing/database/database_helper.h"
#include "routing/database/db_graph.h"
#include "routing/database/db_edge_iterator.h"
#include "routing/utility/point.h"
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
#include "routing/types.h"

#include "routing/table_names.h"

#include <string>

namespace routing {
namespace query {

/**
 * AlgorithmFactory for Dijkstra's algorithm.
 * Dijkstra has no preprocessing so it is always used with DynamicProfileMode.
 * That means that road graph edge lengths must be based on a profile so that
 * they can be swapped at moments notice by swapping the underlying profile.
 */
class DijkstraFactory {
public:
    using EdgeFactory = BasicProfileEdgeFactory;
    using Edge = EdgeFactory::Edge;
    using EndpointEdgeFactory = ProfileEndpointEdgeFactory<Edge>;
    using EdgeRange = VectorEdgeRange<Edge>;
    using Vertex = BasicVertex<Edge, EdgeRange>;
    using DbGraph = database::UnpreprocessedDbGraph;

    using Graph = AdjacencyListGraph<Vertex, Edge>;
    using Algorithm = Dijkstra<RoutingGraph<Graph>>;
    using EndpointAlgorithmPolicy = EndpointAlgorithmPolicyDijkstra<RoutingGraph<Graph>, EdgeRangePolicyVector<Edge>>;

    DijkstraFactory() : endpoint_edges_lengths_() {}

    /**
     * Load graph from database.
     */
    static Graph CreateGraph(database::DatabaseHelper& d, TableNames* table_names, DynamicLengthSource* length_source) {
        Graph g{};
        DbGraph db_graph{};
        EdgeFactory edge_factory{length_source};
        d.LoadGraphEdges<Graph>(table_names->GetEdgesTable(), g, &db_graph, edge_factory);
        return g;
    }

    EndpointEdgesCreator<EndpointEdgeFactory> CreateEndpointEdgesCreator(database::DatabaseHelper& d, database::DbGraph* db_graph) {
        return EndpointEdgesCreator<EndpointEdgeFactory>{d, db_graph, EndpointEdgeFactory{&endpoint_edges_lengths_}};
    }

    EndpointAlgorithmPolicy CreateEndpointAlgorithmPolicy(RoutingGraph<Graph>& routing_graph) {
        return EndpointAlgorithmPolicy{routing_graph, EdgeRangePolicyVector<Edge>{}};
    }

    DbGraph CreateDbGraph() {
        return DbGraph{};
    }
private:
    EndpointEdgesLengths endpoint_edges_lengths_;
};

/**
 * AlgorithmFactory for Contraction Hierarchies algorithm use with StaticProfileMode.
 * It uses graphs whose lengths are set in stone. No profiles and their preference
 * indices that contain actual values need to be loaded to do routing.
 * It saves memory.
 */
class CHStaticFactory {
public:
    using EdgeFactory = CHNumberEdgeFactory;
    using Edge = EdgeFactory::Edge;
    using EndpointEdgeFactory = NumberEndpointEdgeFactory<Edge>;
    using EdgeIterator = std::vector<Edge>::iterator;
    using EdgeRange = IteratorEdgeRange<Edge, EdgeIterator>;
    using Vertex = CHVertex<Edge, EdgeRange>;
    
    using TemporaryGraph = BidirectionalGraph<AdjacencyListGraph<CHVertex<Edge, VectorEdgeRange<Edge>>, Edge>>;
    using Graph = CHSearchGraph<Vertex, Edge>;
    using DbGraph = database::CHDbGraph;
    using Algorithm = BidirectionalDijkstra<RoutingGraph<Graph>>;
    using EndpointAlgorithmPolicy = EndpointAlgorithmPolicyContractionHierarchies<RoutingGraph<Graph>, EdgeRangePolicyVectorIterator<Edge>>;

    CHStaticFactory() {}

    /**
     * Load graph from database and then create an immutable search graph with is more 
     * memory efficient (has only half edges).
     */
    static Graph CreateGraph(database::DatabaseHelper& d, TableNames* table_names) {
        TemporaryGraph g{};
        DbGraph db_graph{};
        EdgeFactory edge_factory{};
        d.LoadGraphEdges<TemporaryGraph>(table_names->GetEdgesTable(), g, &db_graph, edge_factory);
        d.LoadAdditionalVertexProperties(table_names->GetVerticesTable(), g);
        Graph search_graph{};
        search_graph.Load(g);
        return search_graph;
    }

    DbGraph CreateDbGraph() {
        return DbGraph{};
    }

    EndpointEdgesCreator<EndpointEdgeFactory> CreateEndpointEdgesCreator(database::DatabaseHelper& d, database::DbGraph* db_graph) {
        return EndpointEdgesCreator<EndpointEdgeFactory>{d, db_graph, EndpointEdgeFactory{}};
    }

    EndpointAlgorithmPolicy CreateEndpointAlgorithmPolicy(RoutingGraph<Graph>& routing_graph) {
        return EndpointAlgorithmPolicy{routing_graph, EdgeRangePolicyVectorIterator<Edge>{}};
    }
};

/**
 * AlgorithmFactory for Contraction Hiearchies algorithm used with DynamicProfileMode.
 * That means that road graph edge lengths must be based on a profile so that
 * they can be swapped at moments notice by swapping the underlying profile.
 */
class CHDynamicFactory {
public:
    using EdgeFactory = CHProfileEdgeFactory;
    using Edge = EdgeFactory::Edge;
    using EndpointEdgeFactory =  ProfileEndpointEdgeFactory<Edge>;
    using EdgeIterator = std::vector<Edge>::iterator;
    using EdgeRange = IteratorEdgeRange<Edge, EdgeIterator>;
    using Vertex = CHVertex<Edge, EdgeRange>;
    
    using TemporaryGraph = BidirectionalGraph<AdjacencyListGraph<CHVertex<Edge, VectorEdgeRange<Edge>>, Edge>>;
    using Graph = CHSearchGraph<Vertex, Edge>;
    using DbGraph = database::CHDbGraph;
    using Algorithm = BidirectionalDijkstra<RoutingGraph<Graph>>;
    using EndpointAlgorithmPolicy = EndpointAlgorithmPolicyContractionHierarchies<RoutingGraph<Graph>, EdgeRangePolicyVectorIterator<Edge>>;

    CHDynamicFactory() : endpoint_edges_lengths_() {}

    /**
     * Load graph from database and then create an immutable search graph with is more 
     * memory efficient (has only half edges).
     */
    static Graph CreateGraph(database::DatabaseHelper& d, TableNames* table_names, DynamicLengthSource* length_source) {
        TemporaryGraph g{};
        DbGraph db_graph{};
        EdgeFactory edge_factory{length_source};
        d.LoadGraphEdges<TemporaryGraph>(table_names->GetEdgesTable(), g, &db_graph, edge_factory);
        d.LoadAdditionalVertexProperties(table_names->GetVerticesTable(), g);
        Graph search_graph{};
        search_graph.Load(g);
        return search_graph;
    }

    DbGraph CreateDbGraph() {
        return DbGraph{};
    }

    EndpointEdgesCreator<EndpointEdgeFactory> CreateEndpointEdgesCreator(database::DatabaseHelper& d, database::DbGraph* db_graph) {
        return EndpointEdgesCreator<EndpointEdgeFactory>{d, db_graph, EndpointEdgeFactory{&endpoint_edges_lengths_}};
    }

    EndpointAlgorithmPolicy CreateEndpointAlgorithmPolicy(RoutingGraph<Graph>& routing_graph) {
        return EndpointAlgorithmPolicy{routing_graph, EdgeRangePolicyVectorIterator<Edge>{}};
    }
private:
    EndpointEdgesLengths endpoint_edges_lengths_;
};





}
}
#endif //ROUTING_QUERY_ALGORITHM_FACTORY_H