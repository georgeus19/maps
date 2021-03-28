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
#include "routing/bidirectional_graph.h"
#include "routing/basic_edge_endpoint_handler.h"
#include "routing/preprocessing/ch_dijkstra.h"
#include "tests/graph_test.h"
#include <string>
#include <vector>
using namespace std;
using namespace routing;
using namespace preprocessing;

using namespace database;
// using namespace testing;
using G = Graph<ContractionVertex<CHPreprocessingEdge>, CHPreprocessingEdge>;

class CHDijkstraTest : public testing::Test {
    protected:
    
    G g_;
    void SetUp() override {
        TestBasicReverseGraph(g_);
    }
};

TEST_F(CHDijkstraTest, LimitedSearch) {
    CHDijkstra<G> dijkstra{g_};
    double max_cost = 4; // Shortest path found to vertices 1, 2, 3, 4.
    dijkstra.Run(1, 7, typename CHDijkstra<G>::SearchRangeLimits{max_cost, 20});
    
    EXPECT_EQ(dijkstra.GetPathLength(1), 0);
    EXPECT_EQ(dijkstra.GetPathLength(2), 2);
    EXPECT_EQ(dijkstra.GetPathLength(3), 2);
    EXPECT_EQ(dijkstra.GetPathLength(4), 5);
    EXPECT_EQ(dijkstra.GetPathLength(5), std::numeric_limits<double>::max());
    EXPECT_EQ(dijkstra.GetPathLength(6), 10);
}

TEST_F(CHDijkstraTest, LimitedSearchWithHopLimit) {
    CHDijkstra<G> dijkstra{g_};
    double max_cost = 100; // can reach everything with this max cost.
    dijkstra.Run(1, 7, typename CHDijkstra<G>::SearchRangeLimits{max_cost, 2});

    EXPECT_EQ(dijkstra.GetPathLength(1), 0);
    EXPECT_EQ(dijkstra.GetPathLength(2), 2);
    EXPECT_EQ(dijkstra.GetPathLength(3), 2);
    EXPECT_EQ(dijkstra.GetPathLength(4), 5);
    EXPECT_EQ(dijkstra.GetPathLength(5), std::numeric_limits<double>::max());
    EXPECT_EQ(dijkstra.GetPathLength(6), 10);
}
