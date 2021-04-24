#include "gtest/gtest.h"
#include "gmock/gmock.h"  
#include "routing/adjacency_list_graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/algorithm.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/vertices/contraction_vertex.h"
#include "routing/query/dijkstra.h"
#include "routing/exception.h"
#include "database/database_helper.h"
#include "utility/point.h"
#include "routing/endpoint_handler.h"
#include "routing/bidirectional_graph.h"
#include "routing/basic_edge_endpoint_handler.h"
#include "tests/graph_test.h"
#include "routing/vertices/ch_vertex.h"
#include "routing/edge_ranges/vector_edge_range.h"
#include <string>
#include <vector>
using namespace std;
using namespace routing;
using namespace query;
using namespace database;
// using namespace testing;
using G = AdjacencyListGraph<BasicVertex<BasicEdge, VectorEdgeRange<BasicEdge>>, BasicEdge>;

class DijkstraTest : public testing::Test {
    protected:
    
    G g_;
    void SetUp() override {
        TestBasicGraph(g_);
    }
};

TEST_F(DijkstraTest, ExistingPath) {
    Algorithm<Dijkstra<G>> alg{g_};
    alg.Run(1, 6);
    vector<Dijkstra<G>::Edge> path = alg.GetRoute();
    for(auto&& e : path) {
        e.Print();
    }

    vector<Dijkstra<G>::Edge> expected_path{
        BasicEdge{1, 1, 3, 2}, BasicEdge{3, 3, 4, 3}, BasicEdge{5, 4, 5, 2}, BasicEdge{7, 5, 6, 2}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}

TEST_F(DijkstraTest, RunTwiceExistingPath) {
    Algorithm<Dijkstra<G>> alg{g_};
    alg.Run(2, 3);
    alg.Run(1, 6);
    vector<Dijkstra<G>::Edge> path = alg.GetRoute();
    for(auto&& e : path) {
        e.Print();
    }

    vector<Dijkstra<G>::Edge> expected_path{
        BasicEdge{1, 1, 3, 2}, BasicEdge{3, 3, 4, 3}, BasicEdge{5, 4, 5, 2}, BasicEdge{7, 5, 6, 2}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}

TEST_F(DijkstraTest, LimitedSearch) {
    // Dijkstra<G> dijkstra{g_};
    // double max_cost = 4; // Shortest path found to vertices 1, 2, 3, 4.
    // dijkstra.Run(1, [=](BasicVertex<BasicEdge>* v) { 
    //         return v->get_cost() > max_cost;
    //     }, [](BasicVertex<BasicEdge>*) { return false; });
    // EXPECT_EQ(dijkstra.GetPathLength(1), 0);
    // EXPECT_EQ(dijkstra.GetPathLength(2), 2);
    // EXPECT_EQ(dijkstra.GetPathLength(3), 2);
    // EXPECT_EQ(dijkstra.GetPathLength(4), 5);
    // EXPECT_EQ(dijkstra.GetPathLength(5), std::numeric_limits<double>::max());
    // EXPECT_EQ(dijkstra.GetPathLength(6), 10);
}


TEST_F(DijkstraTest, NotExistingPath) {
    Algorithm<Dijkstra<G>> alg{g_};
    EXPECT_THROW(alg.Run(5, 1), RouteNotFoundException);
}

