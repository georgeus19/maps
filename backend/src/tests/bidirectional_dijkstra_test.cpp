#include "gtest/gtest.h"
#include "gmock/gmock.h"  
#include "routing/adjacency_list_graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/algorithm.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/vertices/contraction_vertex.h"
#include "routing/query/dijkstra.h"
#include "routing/bidirectional_graph.h"
#include "routing/vertices/contraction_search_vertex.h"
#include "routing/query/bidirectional_dijkstra.h"
#include "routing/preprocessing/graph_contractor.h"
#include "routing/exception.h"
#include "database/database_helper.h"
#include "utility/point.h"
#include "routing/endpoint_handler.h"
#include "routing/basic_edge_endpoint_handler.h"
#include "tests/graph_test.h"
#include "routing/edges/ch_search_edge.h"
#include "routing/ch_search_graph.h"
#include "routing/vertices/ch_search_vertex.h"
#include "routing/vertices/ch_vertex.h"
#include "routing/edge_ranges/vector_edge_range.h"
#include "routing/edge_ranges/iterator_edge_range.h"

#include <string>
#include <vector>

using namespace std;
using namespace routing;
using namespace database;
using namespace query;
using namespace preprocessing;
// using namespace testing;
// using G = BidirectionalGraph<AdjacencyListGraph<ContractionSearchVertex<CHSearchEdge>, CHSearchEdge>>;
using G = BidirectionalGraph<AdjacencyListGraph<CHVertex<CHSearchEdge, VectorEdgeRange<CHSearchEdge>>, CHSearchEdge>>;
// using SearchGraph = CHSearchGraph<CHSearchVertex<CHSearchEdge, typename std::vector<CHSearchEdge>::iterator>, CHSearchEdge>;
using SearchGraph = CHSearchGraph<CHVertex<CHSearchEdge, IteratorEdgeRange<CHSearchEdge, std::vector<CHSearchEdge>::iterator>>, CHSearchEdge>;

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
        CHSearchEdge{1, 1, 3, 2}, CHSearchEdge{1, 3, 4, 3}, CHSearchEdge{5, 4, 5, 2}, CHSearchEdge{7, 5, 6, 2}
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
        CHSearchEdge{1, 1, 4, 5}, CHSearchEdge{5, 4, 5, 2}, CHSearchEdge{7, 5, 6, 2}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}

TEST(BidirectionalDijkstraTestsNotFixture, IngoreDeadQueueMembers) {
    G g{};
    g.AddEdge(std::move(routing::CHSearchEdge{0, 1, 5, 20}));

    g.AddEdge(std::move(routing::CHSearchEdge{1, 2, 3, 4}));
    g.AddEdge(std::move(routing::CHSearchEdge{2, 2, 4, 1}));

    g.AddEdge(std::move(routing::CHSearchEdge{3, 4, 3, 2}));
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
        CHSearchEdge{1, 1, 2, 1}, CHSearchEdge{1, 2, 3, 2}, CHSearchEdge{1, 3, 4, 3}, CHSearchEdge{1, 4, 5, 4},
        CHSearchEdge{1, 5, 6, 5}, CHSearchEdge{1, 6, 7, 4}, CHSearchEdge{1, 7, 8, 3}, CHSearchEdge{1, 8, 9, 2},
        CHSearchEdge{1, 9, 10, 1}
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
        CHSearchEdge{1, 1, 2, 1}, CHSearchEdge{1, 2, 3, 2}, CHSearchEdge{1, 3, 4, 3}, CHSearchEdge{1, 4, 5, 4},
        CHSearchEdge{1, 5, 6, 5}, CHSearchEdge{1, 6, 7, 4}, CHSearchEdge{1, 7, 8, 3}, CHSearchEdge{1, 8, 9, 2},
        CHSearchEdge{1, 9, 10, 1}
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
        CHSearchEdge{1, 4, 5, 4}, CHSearchEdge{1, 5, 6, 5}, CHSearchEdge{1, 6, 7, 4},
        CHSearchEdge{1, 7, 8, 3}, CHSearchEdge{1, 8, 9, 2}, CHSearchEdge{1, 9, 10, 1}
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
        CHSearchEdge{1, 1, 2, 1}, CHSearchEdge{1, 2, 3, 2}, CHSearchEdge{1, 3, 4, 3}, CHSearchEdge{1, 4, 5, 4},
        CHSearchEdge{1, 5, 6, 5}, CHSearchEdge{1, 6, 7, 4}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}

