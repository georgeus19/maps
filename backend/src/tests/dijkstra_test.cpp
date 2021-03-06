#include "gtest/gtest.h"
#include "gmock/gmock.h"  
#include "routing/adjacency_list_graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/algorithm.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/query/dijkstra.h"
#include "routing/exception.h"
#include "routing/database/database_helper.h"
#include "routing/utility/point.h"
#include "routing/bidirectional_graph.h"
#include "tests/graph_test.h"
#include "routing/vertices/ch_vertex.h"
#include "routing/edge_ranges/vector_edge_range.h"
#include "routing/edges/length_source.h"
#include "routing/types.h"

#include <string>
#include <vector>
using namespace std;
using namespace routing;
using namespace query;
using namespace database;
// using namespace testing;
using Edge = BasicEdge<NumberLengthSource>;
using G = AdjacencyListGraph<BasicVertex<Edge, VectorEdgeRange<Edge>>, Edge>;

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
        Edge{1, 1, 3, 2}, Edge{3, 3, 4, 3}, Edge{5, 4, 5, 2}, Edge{7, 5, 6, 2}
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
        Edge{1, 1, 3, 2}, Edge{3, 3, 4, 3}, Edge{5, 4, 5, 2}, Edge{7, 5, 6, 2}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}


TEST_F(DijkstraTest, NotExistingPath) {
    Algorithm<Dijkstra<G>> alg{g_};
    EXPECT_THROW(alg.Run(5, 1), RouteNotFoundException);
}

