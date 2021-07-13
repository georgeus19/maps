#include "gtest/gtest.h"
#include "gmock/gmock.h"  
#include "routing/adjacency_list_graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/algorithm.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/query/dijkstra.h"
#include "routing/bidirectional_graph.h"
#include "routing/query/bidirectional_dijkstra.h"
#include "routing/preprocessing/graph_contractor.h"
#include "routing/exception.h"
#include "routing/database/database_helper.h"
#include "routing/utility/point.h"
#include "tests/graph_test.h"
#include "routing/edges/ch_edge.h"
#include "routing/ch_search_graph.h"
#include "routing/vertices/ch_vertex.h"
#include "routing/edge_ranges/vector_edge_range.h"
#include "routing/edge_ranges/iterator_edge_range.h"
#include "routing/edges/length_source.h"
#include "routing/routing_graph.h"

#include <string>
#include <vector>

using namespace std;
using namespace routing;
using namespace database;
using namespace query;
using namespace preprocessing;
// using namespace testing;
using Edge = CHEdge<NumberLengthSource>;
using G = BidirectionalGraph<AdjacencyListGraph<CHVertex<Edge, VectorEdgeRange<Edge>>, Edge>>;
using EdgeRange = IteratorEdgeRange<Edge, std::vector<Edge>::iterator>;
using SearchVertex = CHVertex<Edge, EdgeRange>;
using SearchGraph = CHSearchGraph<SearchVertex, Edge>;

TEST(RoutingGraphTests, OverlapVertices) {
    G load_graph;
	load_graph.AddEdge(std::move(typename G::Edge{0, 1, 2, 2, G::Edge::EdgeType::forward}));
    load_graph.AddEdge(std::move(typename G::Edge{1, 1, 3, 3, G::Edge::EdgeType::twoway}));
	load_graph.AddEdge(std::move(typename G::Edge{2, 2, 6, 12, G::Edge::EdgeType::forward}));
	load_graph.AddEdge(std::move(typename G::Edge{3, 3, 4, 2, G::Edge::EdgeType::twoway}));
	load_graph.AddEdge(std::move(typename G::Edge{4, 4, 5, 3, G::Edge::EdgeType::twoway}));
	load_graph.AddEdge(std::move(typename G::Edge{5, 4, 6, 6, G::Edge::EdgeType::forward}));
	load_graph.AddEdge(std::move(typename G::Edge{6, 5, 6, 2, G::Edge::EdgeType::forward}));
	load_graph.AddEdge(std::move(typename G::Edge{7, 6, 5, 3, G::Edge::EdgeType::forward}));

	// Shortcuts.
	load_graph.AddEdge(std::move(typename G::Edge{8, 1, 4, 3, G::Edge::EdgeType::twoway, 3}));
	load_graph.AddEdge(std::move(typename G::Edge{9, 1, 5, 8, G::Edge::EdgeType::twoway, 4}));
	load_graph.AddEdge(std::move(typename G::Edge{10, 1, 6, 11, G::Edge::EdgeType::forward, 4}));
	load_graph.AddEdge(std::move(typename G::Edge{11, 5, 2, 10, G::Edge::EdgeType::forward, 1}));
	load_graph.AddEdge(std::move(typename G::Edge{12, 2, 5, 15, G::Edge::EdgeType::forward, 6}));

	load_graph.GetVertex(1).set_ordering_rank(10);   
	load_graph.GetVertex(2).set_ordering_rank(7);   
	load_graph.GetVertex(3).set_ordering_rank(3);   
	load_graph.GetVertex(4).set_ordering_rank(4);   
	load_graph.GetVertex(5).set_ordering_rank(8);   
	load_graph.GetVertex(6).set_ordering_rank(11);   
    SearchGraph search_graph{};
    search_graph.Load(load_graph);
    std::vector<Edge> additional_edges{
        Edge{18, 1, 3, 4, Edge::EdgeType::forward},
        Edge{14, 1, 4, 10, Edge::EdgeType::forward},
        Edge{13, 1, 3, 2, Edge::EdgeType::forward},
        Edge{15, 6, 5, 1, Edge::EdgeType::backward},
        Edge{16, 6, 5, 2, Edge::EdgeType::backward},
        Edge{17, 6, 4, 20, Edge::EdgeType::backward}
    };
    RoutingGraph<SearchGraph> routing_graph{search_graph};
    auto source_begin_it = additional_edges.begin();
    auto source_end_it = additional_edges.begin();
    std::advance(source_end_it, 3);
    auto target_begin_it = additional_edges.begin();
    std::advance(target_begin_it, 3);
    auto target_end_it = additional_edges.end();
    routing_graph.AddVertex(SearchVertex{1, EdgeRange{source_begin_it, source_end_it}, 1});
    routing_graph.AddVertex(SearchVertex{6, EdgeRange{target_begin_it, target_end_it}, 2});
 
    Algorithm<BidirectionalDijkstra<RoutingGraph<SearchGraph>>> alg{routing_graph};
    alg.Run(1, 6);
    std::vector<Dijkstra<RoutingGraph<SearchGraph>>::Edge> path = alg.GetRoute();
    for(auto&& e : path) {
        e.Print();
    }

    std::vector<Dijkstra<RoutingGraph<SearchGraph>>::Edge> expected_path {
        Edge{1, 1, 3, 2, Edge::EdgeType::forward},
        Edge{1, 3, 4, 2, Edge::EdgeType::twoway},
        Edge{1, 4, 5, 3, Edge::EdgeType::twoway},
        Edge{1, 5, 6, 1, Edge::EdgeType::backward}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}

TEST(RoutingGraphTests, AdditionalVertices) {
    G load_graph;
	load_graph.AddEdge(std::move(typename G::Edge{2, 2, 6, 12, G::Edge::EdgeType::forward}));
	load_graph.AddEdge(std::move(typename G::Edge{3, 3, 4, 2, G::Edge::EdgeType::twoway}));
	load_graph.AddEdge(std::move(typename G::Edge{4, 4, 5, 3, G::Edge::EdgeType::twoway}));
	load_graph.AddEdge(std::move(typename G::Edge{5, 4, 6, 6, G::Edge::EdgeType::forward}));
	load_graph.AddEdge(std::move(typename G::Edge{6, 5, 6, 2, G::Edge::EdgeType::forward}));
	load_graph.AddEdge(std::move(typename G::Edge{7, 6, 5, 3, G::Edge::EdgeType::forward}));

	// Shortcuts.
	load_graph.AddEdge(std::move(typename G::Edge{11, 5, 2, 10, G::Edge::EdgeType::forward, 1}));
	load_graph.AddEdge(std::move(typename G::Edge{12, 2, 5, 15, G::Edge::EdgeType::forward, 6}));

	load_graph.GetVertex(2).set_ordering_rank(7);   
	load_graph.GetVertex(3).set_ordering_rank(3);   
	load_graph.GetVertex(4).set_ordering_rank(4);   
	load_graph.GetVertex(5).set_ordering_rank(8);   
	load_graph.GetVertex(6).set_ordering_rank(6);   
    SearchGraph search_graph{};
    search_graph.Load(load_graph);
    std::vector<Edge> additional_edges{
        Edge{18, 1, 3, 4, Edge::EdgeType::forward},
        Edge{13, 1, 3, 3, Edge::EdgeType::forward},
        Edge{14, 1, 4, 10, Edge::EdgeType::forward},
        Edge{15, 7, 6, 2, Edge::EdgeType::backward},
        Edge{16, 7, 6, 6, Edge::EdgeType::backward},
        Edge{17, 7, 5, 13, Edge::EdgeType::backward}
    };
    RoutingGraph<SearchGraph> routing_graph{search_graph};
    auto source_begin_it = additional_edges.begin();
    auto source_end_it = additional_edges.begin();
    std::advance(source_end_it, 3);
    auto target_begin_it = additional_edges.begin();
    std::advance(target_begin_it, 3);
    auto target_end_it = additional_edges.end();
    routing_graph.AddVertex(SearchVertex{1, EdgeRange{source_begin_it, source_end_it}, 1});
    routing_graph.AddVertex(SearchVertex{7, EdgeRange{target_begin_it, target_end_it}, 2});
 
    Algorithm<BidirectionalDijkstra<RoutingGraph<SearchGraph>>> alg{routing_graph};
    alg.Run(1, 7);
    std::vector<Dijkstra<RoutingGraph<SearchGraph>>::Edge> path = alg.GetRoute();
    for(auto&& e : path) {
        e.Print();
    }

    std::vector<Dijkstra<RoutingGraph<SearchGraph>>::Edge> expected_path {
        Edge{1, 1, 3, 3, Edge::EdgeType::forward},
        Edge{1, 3, 4, 2, Edge::EdgeType::twoway},
        Edge{1, 4, 5, 3, Edge::EdgeType::twoway},
        Edge{1, 5, 6, 2, Edge::EdgeType::backward},
        Edge{15, 6, 7, 2, Edge::EdgeType::backward}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}
