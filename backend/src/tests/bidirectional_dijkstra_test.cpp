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
#include <string>
#include <vector>
using namespace std;
using namespace routing;
using namespace database;
using namespace preprocessing;
// using namespace testing;
using G = Graph<ContractionSearchVertex<BasicEdge>, BasicEdge>;

class BidirectionalDijkstraTest : public testing::Test {
    protected:
    
    G g_;
    void SetUp() override {
        TestBasicContractedGraph(g_);
        g_.GetVertex(1).set_ordering_rank(3);
        g_.GetVertex(2).set_ordering_rank(5);
        g_.GetVertex(3).set_ordering_rank(1);
        g_.GetVertex(4).set_ordering_rank(6);
        g_.GetVertex(5).set_ordering_rank(4);
        g_.GetVertex(6).set_ordering_rank(2);
    }
};

TEST_F(BidirectionalDijkstraTest, ExistingPath) {
    Algorithm<BidirectionalDijkstra<G>> alg{g_};
    alg.Run(1, 6);
    vector<Dijkstra<G>::Edge> path = alg.GetRoute();
    for(auto&& e : path) {
        e.Print();
    }

    vector<Dijkstra<G>::Edge> expected_path{
        BasicEdge{1, 1, 4, 5}, BasicEdge{5, 4, 5, 2}, BasicEdge{7, 5, 6, 2}
    };

    EXPECT_THAT(path, testing::ElementsAreArray(expected_path));
}

TEST_F(BidirectionalDijkstraTest, NotExistingPath) {
    Algorithm<BidirectionalDijkstra<G>> alg{g_};
    EXPECT_THROW(alg.Run(5, 1), RouteNotFoundException);
}

