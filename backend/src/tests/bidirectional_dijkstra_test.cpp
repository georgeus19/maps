#include "gtest/gtest.h"
#include "gmock/gmock.h"  
#include "routing/graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/algorithm.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/vertices/contraction_vertex.h"
#include "routing/dijkstra.h"
#include "routing/vertices/contraction_search_vertex.h"
#include "routing/preprocessing/bidirectional_dijkstra.h"
#include "routing/preprocessing/graph_contractor.h"
#include "routing/exception.h"
#include "database/database_helper.h"
#include "utility/point.h"
#include "routing/endpoint_handler.h"
#include "routing/basic_edge_endpoint_handler.h"
#include "tests/graph_test.h"
#include "routing/edges/contraction_edge.h"

#include <string>
#include <vector>

using namespace std;
using namespace routing;
using namespace database;
using namespace preprocessing;
// using namespace testing;
using G = Graph<ContractionSearchVertex<ContractionEdge>, ContractionEdge>;

class BidirectionalDijkstraTests : public testing::Test {
    protected:
    
    G g_;
    void SetUp() override {
        TestBasicContractedGraph(g_);
    }
};

TEST_F(BidirectionalDijkstraTests, ExistingPathWithShortcuts) {
    Algorithm<BidirectionalDijkstra<G>> alg{g_};
    alg.Run(1, 6);
    vector<Dijkstra<G>::Edge> path = alg.GetRoute();
    for(auto&& e : path) {
        e.Print();
    }

    vector<Dijkstra<G>::Edge> expected_path {
        ContractionEdge{1, 1, 3, 2}, ContractionEdge{1, 3, 4, 3}, ContractionEdge{5, 4, 5, 2}, ContractionEdge{7, 5, 6, 2}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}



TEST_F(BidirectionalDijkstraTests, NotExistingPath) {
    Algorithm<BidirectionalDijkstra<G>> alg{g_};
    EXPECT_THROW(alg.Run(5, 1), RouteNotFoundException);
}

TEST(BidirectionalDijkstraTestsNotFixture, ExistingPathWithoutShortcuts) {
    G g;
    TestBasicContractedGraph(g, false);
    Algorithm<BidirectionalDijkstra<G>> alg{g};
    alg.Run(1, 6);
    vector<Dijkstra<G>::Edge> path = alg.GetRoute();
    for(auto&& e : path) {
        e.Print();
    }

    vector<Dijkstra<G>::Edge> expected_path {
        ContractionEdge{1, 1, 4, 5}, ContractionEdge{5, 4, 5, 2}, ContractionEdge{7, 5, 6, 2}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}

TEST(BidirectionalDijkstraTestsNotFixture, IngoreDeadQueueMembers) {
    G g{};
    g.AddEdge(std::move(routing::ContractionEdge{0, 1, 5, 20}));
    g.AddReverseEdge(std::move(routing::ContractionEdge{0, 1, 5, 20}));

    g.AddEdge(std::move(routing::ContractionEdge{1, 2, 3, 4}));
    g.AddReverseEdge(std::move(routing::ContractionEdge{1, 2, 3, 4}));
    g.AddEdge(std::move(routing::ContractionEdge{2, 2, 4, 1}));
    g.AddReverseEdge(std::move(routing::ContractionEdge{2, 2, 4, 1}));

    g.AddEdge(std::move(routing::ContractionEdge{3, 4, 3, 2}));
    g.AddReverseEdge(std::move(routing::ContractionEdge{3, 4, 3, 2}));
    g.GetVertex(1).set_ordering_rank(1);
    g.GetVertex(2).set_ordering_rank(3);
    g.GetVertex(3).set_ordering_rank(5);
    g.GetVertex(4).set_ordering_rank(4);
    g.GetVertex(5).set_ordering_rank(2);
    Algorithm<BidirectionalDijkstra<G>> alg{g};
    EXPECT_THROW(alg.Run(2, 1), RouteNotFoundException);
}

TEST(BidirectionalDijkstraTestsNotFixture, PathGraph) {
    G g;
    TestPathGraph(g);
 
    Algorithm<BidirectionalDijkstra<G>> alg{g};
    alg.Run(1, 10);
    vector<Dijkstra<G>::Edge> path = alg.GetRoute();
    for(auto&& e : path) {
        e.Print();
    }

    vector<Dijkstra<G>::Edge> expected_path {
        ContractionEdge{1, 1, 2, 1}, ContractionEdge{1, 2, 3, 2}, ContractionEdge{1, 3, 4, 3}, ContractionEdge{1, 4, 5, 4},
        ContractionEdge{1, 5, 6, 5}, ContractionEdge{1, 6, 7, 4}, ContractionEdge{1, 7, 8, 3}, ContractionEdge{1, 8, 9, 2},
        ContractionEdge{1, 9, 10, 1}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}

TEST(BidirectionalDijkstraTestsNotFixture, PathShortcutGraph) {
    G g;
    TestPathShortcutGraph(g);
 
    Algorithm<BidirectionalDijkstra<G>> alg{g};
    alg.Run(1, 10);
    vector<Dijkstra<G>::Edge> path = alg.GetRoute();
    for(auto&& e : path) {
        e.Print();
    }

    vector<Dijkstra<G>::Edge> expected_path {
        ContractionEdge{1, 1, 2, 1}, ContractionEdge{1, 2, 3, 2}, ContractionEdge{1, 3, 4, 3}, ContractionEdge{1, 4, 5, 4},
        ContractionEdge{1, 5, 6, 5}, ContractionEdge{1, 6, 7, 4}, ContractionEdge{1, 7, 8, 3}, ContractionEdge{1, 8, 9, 2},
        ContractionEdge{1, 9, 10, 1}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}

TEST(BidirectionalDijkstraTestsNotFixture, PathShortcutGraphRightForwardRecursion) {
    G g;
    TestPathShortcutGraph(g);
    g.GetVertex(4).set_ordering_rank(9);
    g.GetVertex(5).set_ordering_rank(10);
    g.GetVertex(6).set_ordering_rank(11);
    g.GetVertex(7).set_ordering_rank(12);
    g.GetVertex(8).set_ordering_rank(13);
    g.GetVertex(9).set_ordering_rank(14);
    g.GetVertex(10).set_ordering_rank(15);
 
    Algorithm<BidirectionalDijkstra<G>> alg{g};
    alg.Run(4, 10);
    vector<Dijkstra<G>::Edge> path = alg.GetRoute();
    for(auto&& e : path) {
        e.Print();
    }

    vector<Dijkstra<G>::Edge> expected_path {
        ContractionEdge{1, 4, 5, 4}, ContractionEdge{1, 5, 6, 5}, ContractionEdge{1, 6, 7, 4},
        ContractionEdge{1, 7, 8, 3}, ContractionEdge{1, 8, 9, 2}, ContractionEdge{1, 9, 10, 1}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}

TEST(BidirectionalDijkstraTestsNotFixture, PathShortcutGraphRightBackwardRecursion) {
    G g;
    TestPathShortcutGraph(g);
    g.GetVertex(1).set_ordering_rank(29);
    g.GetVertex(2).set_ordering_rank(28);
    g.GetVertex(3).set_ordering_rank(27);
    g.GetVertex(4).set_ordering_rank(26);
    g.GetVertex(5).set_ordering_rank(25);
    g.GetVertex(6).set_ordering_rank(24);
    g.GetVertex(7).set_ordering_rank(23);
 
    Algorithm<BidirectionalDijkstra<G>> alg{g};
    alg.Run(1, 7);
    vector<Dijkstra<G>::Edge> path = alg.GetRoute();
    for(auto&& e : path) {
        e.Print();
    }

    vector<Dijkstra<G>::Edge> expected_path {
        ContractionEdge{1, 1, 2, 1}, ContractionEdge{1, 2, 3, 2}, ContractionEdge{1, 3, 4, 3}, ContractionEdge{1, 4, 5, 4},
        ContractionEdge{1, 5, 6, 5}, ContractionEdge{1, 6, 7, 4}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}

