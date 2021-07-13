#include "gtest/gtest.h"
#include "gmock/gmock.h"  
#include "routing/adjacency_list_graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/algorithm.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/query/dijkstra.h"
#include "routing/bidirectional_graph.h"
#include "routing/query/bidirectional_dijkstra.h"
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
#include "routing/types.h"

#include <string>
#include <vector>

using namespace std;
using namespace routing;
using namespace database;
using namespace query;
// using namespace testing;
using Edge = CHEdge<NumberLengthSource>;
using G = BidirectionalGraph<AdjacencyListGraph<CHVertex<Edge, VectorEdgeRange<Edge>>, Edge>>;
using SearchGraph = CHSearchGraph<CHVertex<Edge, IteratorEdgeRange<Edge, std::vector<Edge>::iterator>>, Edge>;

class BidirectionalDijkstraTests : public testing::Test {
    protected:
    
    SearchGraph g_;
    void SetUp() override {
        G g{};
        TestBasicContractedGraph(g);
        g_.Load(g);
    }
};

TEST_F(BidirectionalDijkstraTests, ExistingPathWithShortcuts) {
    Algorithm<BidirectionalDijkstra<SearchGraph>> alg{g_};
    alg.Run(1, 6);
    vector<Dijkstra<SearchGraph>::Edge> path = alg.GetRoute();
    for(auto&& e : path) {
        e.Print();
    }

    vector<Dijkstra<SearchGraph>::Edge> expected_path {
        Edge{1, 1, 3, 2, Edge::EdgeType::backward},
        Edge{1, 3, 4, 3, Edge::EdgeType::forward},
        Edge{5, 4, 5, 2, Edge::EdgeType::backward},
        Edge{7, 5, 6, 2, Edge::EdgeType::backward}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}



TEST_F(BidirectionalDijkstraTests, NotExistingPath) {
    Algorithm<BidirectionalDijkstra<SearchGraph>> alg{g_};
    EXPECT_THROW(alg.Run(5, 1), RouteNotFoundException);
}

TEST(BidirectionalDijkstraTestsNotFixture, ExistingPathWithoutShortcuts) {
    SearchGraph search_graph;
    G load_graph{};
    TestBasicContractedGraph(load_graph, false);
    search_graph.Load(load_graph);
    Algorithm<BidirectionalDijkstra<SearchGraph>> alg{search_graph};
    alg.Run(1, 6);
    vector<Dijkstra<G>::Edge> path = alg.GetRoute();
    for(auto&& e : path) {
        e.Print();
    }

    vector<Dijkstra<G>::Edge> expected_path {
        Edge{1, 1, 4, 5, Edge::EdgeType::forward},
        Edge{5, 4, 5, 2, Edge::EdgeType::backward},
        Edge{7, 5, 6, 2, Edge::EdgeType::backward}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}

TEST(BidirectionalDijkstraTestsNotFixture, IngoreDeadQueueMembers) {
    G g{};
    g.AddEdge(Edge{0, 1, 5, 20});

    g.AddEdge(Edge{1, 2, 3, 4});
    g.AddEdge(Edge{2, 2, 4, 1});

    g.AddEdge(Edge{3, 4, 3, 2});
    g.GetVertex(1).set_ordering_rank(1);
    g.GetVertex(2).set_ordering_rank(3);
    g.GetVertex(3).set_ordering_rank(5);
    g.GetVertex(4).set_ordering_rank(4);
    g.GetVertex(5).set_ordering_rank(2);
    Algorithm<BidirectionalDijkstra<G>> alg{g};
    EXPECT_THROW(alg.Run(2, 1), RouteNotFoundException);
}

TEST(BidirectionalDijkstraTestsNotFixture, PathGraph) {
    G load_graph;
    TestPathGraph(load_graph);
    SearchGraph search_graph{};
    search_graph.Load(load_graph);
 
    Algorithm<BidirectionalDijkstra<SearchGraph>> alg{search_graph};
    alg.Run(1, 10);
    vector<Dijkstra<SearchGraph>::Edge> path = alg.GetRoute();
    for(auto&& e : path) {
        e.Print();
    }

    vector<Dijkstra<SearchGraph>::Edge> expected_path {
        Edge{1, 1, 2, 1, Edge::EdgeType::forward},
        Edge{1, 2, 3, 2, Edge::EdgeType::forward},
        Edge{1, 3, 4, 3, Edge::EdgeType::forward},
        Edge{1, 4, 5, 4, Edge::EdgeType::forward},
        Edge{1, 5, 6, 5, Edge::EdgeType::forward},
        Edge{1, 6, 7, 4, Edge::EdgeType::backward},
        Edge{1, 7, 8, 3, Edge::EdgeType::backward},
        Edge{1, 8, 9, 2, Edge::EdgeType::backward},
        Edge{1, 9, 10, 1, Edge::EdgeType::backward}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}

TEST(BidirectionalDijkstraTestsNotFixture, PathShortcutGraph) {
    G load_graph;
    TestPathShortcutGraph(load_graph);
    SearchGraph search_graph{};
    search_graph.Load(load_graph);
 
    Algorithm<BidirectionalDijkstra<SearchGraph>> alg{search_graph};
    alg.Run(1, 10);
    vector<Dijkstra<SearchGraph>::Edge> path = alg.GetRoute();
    for(auto&& e : path) {
        e.Print();
    }

    vector<Dijkstra<SearchGraph>::Edge> expected_path {
        Edge{1, 1, 2, 1, Edge::EdgeType::backward},
        Edge{1, 2, 3, 2, Edge::EdgeType::forward},
        Edge{1, 3, 4, 3, Edge::EdgeType::forward},
        Edge{1, 4, 5, 4, Edge::EdgeType::forward},
        Edge{1, 5, 6, 5, Edge::EdgeType::backward},
        Edge{1, 6, 7, 4, Edge::EdgeType::backward},
        Edge{1, 7, 8, 3, Edge::EdgeType::backward},
        Edge{1, 8, 9, 2, Edge::EdgeType::backward},
        Edge{1, 9, 10, 1, Edge::EdgeType::forward}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}

TEST(BidirectionalDijkstraTestsNotFixture, PathShortcutGraphRightForwardRecursion) {
    G load_graph;
    TestPathShortcutGraph(load_graph);
    load_graph.GetVertex(4).set_ordering_rank(24);
    load_graph.GetVertex(5).set_ordering_rank(25);
    load_graph.GetVertex(6).set_ordering_rank(23);
    load_graph.GetVertex(7).set_ordering_rank(22);
    load_graph.GetVertex(8).set_ordering_rank(21);
    load_graph.GetVertex(9).set_ordering_rank(20);
    load_graph.GetVertex(10).set_ordering_rank(26);
    SearchGraph search_graph{};
    search_graph.Load(load_graph);
 
    Algorithm<BidirectionalDijkstra<SearchGraph>> alg{search_graph};
    alg.Run(4, 10);
    vector<Dijkstra<SearchGraph>::Edge> path = alg.GetRoute();
    for(auto&& e : path) {
        e.Print();
    }

    vector<Dijkstra<SearchGraph>::Edge> expected_path {
        Edge{1, 4, 5, 4, Edge::EdgeType::forward},
        Edge{1, 5, 6, 5, Edge::EdgeType::backward},
        Edge{1, 6, 7, 4, Edge::EdgeType::backward},
        Edge{1, 7, 8, 3, Edge::EdgeType::backward},
        Edge{1, 8, 9, 2, Edge::EdgeType::backward},
        Edge{1, 9, 10, 1, Edge::EdgeType::forward}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}

TEST(BidirectionalDijkstraTestsNotFixture, PathShortcutGraphRightBackwardRecursion) {
    G load_graph;
    TestPathShortcutGraph(load_graph);
    load_graph.GetVertex(1).set_ordering_rank(30);
    load_graph.GetVertex(2).set_ordering_rank(26);
    load_graph.GetVertex(3).set_ordering_rank(27);
    load_graph.GetVertex(4).set_ordering_rank(28);
    load_graph.GetVertex(5).set_ordering_rank(29);
    load_graph.GetVertex(6).set_ordering_rank(25);
    load_graph.GetVertex(7).set_ordering_rank(24);
    SearchGraph search_graph{};
    search_graph.Load(load_graph);
 
    Algorithm<BidirectionalDijkstra<SearchGraph>> alg{search_graph};
    alg.Run(1, 7);
    vector<Dijkstra<SearchGraph>::Edge> path = alg.GetRoute();
    for(auto&& e : path) {
        e.Print();
    }

    vector<Dijkstra<SearchGraph>::Edge> expected_path {
        Edge{1, 1, 2, 1, Edge::EdgeType::backward},
        Edge{1, 2, 3, 2, Edge::EdgeType::forward},
        Edge{1, 3, 4, 3, Edge::EdgeType::forward},
        Edge{1, 4, 5, 4, Edge::EdgeType::forward},
        Edge{1, 5, 6, 5, Edge::EdgeType::backward},
        Edge{1, 6, 7, 4, Edge::EdgeType::backward}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}

TEST(BidirectionalDijkstraTestsNotFixture, GraphWithTwowayEdges) {
    G load_graph;
    TestBidirectedSearchGraph(load_graph);
    SearchGraph search_graph{};
    search_graph.Load(load_graph);
 
    Algorithm<BidirectionalDijkstra<SearchGraph>> alg{search_graph};
    alg.Run(1, 6);
    vector<Dijkstra<SearchGraph>::Edge> path = alg.GetRoute();
    for(auto&& e : path) {
        e.Print();
    }

    vector<Dijkstra<SearchGraph>::Edge> expected_path {
        Edge{1, 1, 3, 3, Edge::EdgeType::twoway}, Edge{1, 3, 4, 2, Edge::EdgeType::twoway},
        Edge{1, 4, 5, 3, Edge::EdgeType::twoway}, Edge{1, 5, 6, 2, Edge::EdgeType::backward}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}
