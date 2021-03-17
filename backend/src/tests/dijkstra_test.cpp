#include "gtest/gtest.h"
#include "gmock/gmock.h"  
#include "routing/graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/algorithm.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/vertices/contraction_vertex.h"
#include "routing/dijkstra.h"
#include "routing/exception.h"
#include "database/database_helper.h"
#include "utility/point.h"
#include "routing/endpoint_handler.h"
#include "routing/basic_edge_endpoint_handler.h"
#include "tests/graph_test.h"
#include <string>
#include <vector>
using namespace std;
using namespace routing;
using namespace database;
// using namespace testing;
using G = Graph<BasicVertex<BasicEdge>, BasicEdge>;

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
    Dijkstra<G> dijkstra{g_};
    double max_cost = 4; // Shortest path found to vertices 1, 2, 3, 4.
    dijkstra.Run(1, [=](BasicVertex<BasicEdge>* v) { 
            return v->get_cost() > max_cost;
        }, [](BasicVertex<BasicEdge>*) { return false; });
    EXPECT_EQ(g_.GetVertex(1).get_cost(), 0);
    EXPECT_EQ(g_.GetVertex(2).get_cost(), 2);
    EXPECT_EQ(g_.GetVertex(3).get_cost(), 2);
    EXPECT_EQ(g_.GetVertex(4).get_cost(), 5);
    EXPECT_EQ(g_.GetVertex(5).get_cost(), std::numeric_limits<double>::max());
    EXPECT_EQ(g_.GetVertex(6).get_cost(), 10);
}


TEST_F(DijkstraTest, NotExistingPath) {
    Algorithm<Dijkstra<G>> alg{g_};
    EXPECT_THROW(alg.Run(5, 1), RouteNotFoundException);
}

