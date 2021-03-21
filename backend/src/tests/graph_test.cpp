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

TEST(GraphTests, ForEachEdgePathGraphTest) {
    G g;
    TestPathGraph(g);
    std::vector<G::E> actual_edges{};
    g.ForEachEdge([&](G::E& edge) {
        actual_edges.push_back(edge);
        edge.Print();
    });
 

    std::vector<G::E> expected_edges{
        ContractionEdge{0, 1, 2, 1},
        ContractionEdge{1, 2, 3, 2},
        ContractionEdge{2, 3, 4, 3},
        ContractionEdge{3, 4, 5, 4},
        ContractionEdge{4, 5, 6, 5},
        ContractionEdge{5, 6, 7, 4},
        ContractionEdge{6, 7, 8, 3},
        ContractionEdge{7, 8, 9, 2},
        ContractionEdge{77, 9, 10, 1}
    };

    EXPECT_THAT(actual_edges, testing::UnorderedElementsAreArray(expected_edges));
}

TEST(GraphTests, ForEachEdgeBasicGraphTest) {
    G g;
    TestBasicReverseGraph(g);
    std::vector<G::E> actual_edges{};
    g.ForEachEdge([&](G::E& edge) {
        actual_edges.push_back(edge);
        edge.Print();
    });
 

    std::vector<G::E> expected_edges{
        ContractionEdge{0, 1, 2, 2},
        ContractionEdge{1, 1, 3, 2},
        ContractionEdge{2, 2, 6, 8},
        ContractionEdge{3, 3, 4, 3},
        ContractionEdge{4, 4, 3, 2},
        ContractionEdge{5, 4, 5, 2},
        ContractionEdge{6, 5, 4, 4},
        ContractionEdge{7, 4, 6, 6},
        ContractionEdge{8, 5, 6, 2},
        ContractionEdge{9, 6, 5, 3},
        ContractionEdge{10, 5, 3, 7}
    };

    EXPECT_THAT(actual_edges, testing::UnorderedElementsAreArray(expected_edges));
}

TEST(GraphTests, ForEachEdgeBasicContractedGraphTest) {
    G g;
    TestBasicContractedGraph(g);
    std::vector<G::E> actual_edges{};
    g.ForEachEdge([&](G::E& edge) {
        actual_edges.push_back(edge);
        edge.Print();
    });
 

    std::vector<G::E> expected_edges{
        ContractionEdge{0, 1, 2, 2},
        ContractionEdge{1, 1, 3, 2},
        ContractionEdge{2, 1, 4, 5, 3},
        ContractionEdge{3, 2, 6, 8},
        ContractionEdge{4, 2, 5, 11, 6},
        ContractionEdge{5, 2, 4, 15, 5},
        ContractionEdge{6, 3, 4, 3},
        ContractionEdge{7, 4, 3, 2},
        ContractionEdge{8, 4, 5, 2},
        ContractionEdge{9, 4, 6, 6},
        ContractionEdge{10, 5, 4, 4},
        ContractionEdge{11, 5, 3, 7},
        ContractionEdge{12, 5, 6, 2},
        ContractionEdge{13, 6, 5, 3}
    };

    EXPECT_THAT(actual_edges, testing::UnorderedElementsAreArray(expected_edges));
}

TEST(GraphTests, ForEachVertexBasicGraphTest) {
    G g;
    TestBasicContractedGraph(g);
    std::vector<G::V> actual_vertices{};
    g.forEachVertex([&](G::V& vertex) {
        actual_vertices.push_back(vertex);
    });
 

    std::vector<G::V> expected_vertices{
        ContractionSearchVertex<ContractionEdge>{1},
        ContractionSearchVertex<ContractionEdge>{2},
        ContractionSearchVertex<ContractionEdge>{3},
        ContractionSearchVertex<ContractionEdge>{4},
        ContractionSearchVertex<ContractionEdge>{5},
        ContractionSearchVertex<ContractionEdge>{6}
    };

    EXPECT_THAT(actual_vertices, testing::UnorderedElementsAreArray(expected_vertices));
}

class GraphGetVertexTests : public testing::TestWithParam<unsigned_id_type> {
    protected:
    
    G g_;
    void SetUp() override {
        TestBasicContractedGraph(g_);
    }
};

INSTANTIATE_TEST_CASE_P(
    GraphGetVertexTestsParameters, 
    GraphGetVertexTests,
    ::testing::Values(
        1, 2, 3, 4, 5, 6
    )
);

TEST_P(GraphGetVertexTests, GetVertexTest) {
    unsigned_id_type vertex_id = GetParam();
    auto&& vertex = g_.GetVertex(vertex_id);
    std::cout << "vertex.id = " << vertex.get_osm_id() << ", expected id = " << vertex_id << std::endl;
    EXPECT_EQ(vertex.get_osm_id(), vertex_id);

}