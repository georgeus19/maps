#include "gtest/gtest.h"
#include "gmock/gmock.h"  
#include "routing/adjacency_list_graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/edges/ch_search_edge.h"
#include "routing/algorithm.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/query/dijkstra.h"
#include "routing/query/bidirectional_dijkstra.h"
#include "routing/preprocessing/graph_contractor.h"
#include "routing/exception.h"
#include "database/database_helper.h"
#include "utility/point.h"
#include "routing/bidirectional_graph.h"
#include "tests/graph_test.h"
#include "routing/edge_ranges/vector_edge_range.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/vertices/ch_vertex.h"

#include <string>
#include <vector>

using namespace std;
using namespace routing;
using namespace database;
using namespace query;
using namespace preprocessing;
// using namespace testing;
using Vertex = CHVertex<CHSearchEdge, VectorEdgeRange<CHSearchEdge>>;
using G = AdjacencyListGraph<Vertex, CHSearchEdge>;

struct GraphForEachEdgeTestsParameter;


class GraphForEachEdgeTests : public testing::TestWithParam<GraphForEachEdgeTestsParameter> {
    protected:
    
    G g_;
};

struct GraphForEachEdgeTestsParameter {
    std::function<void(G& g)> function;
    std::vector<G::Edge> expected_edges;

    GraphForEachEdgeTestsParameter(const std::function<void(G& g)>& f, const std::vector<G::Edge>& e) 
        : function(f), expected_edges(e) {}
};

INSTANTIATE_TEST_CASE_P(
    GraphForEachEdgeTestsParameters, 
    GraphForEachEdgeTests,
    ::testing::Values(
        GraphForEachEdgeTestsParameter{
            [](G& g){ TestPathGraph(g); },
            std::vector<G::Edge> {
                CHSearchEdge{0, 1, 2, 1},
                CHSearchEdge{1, 2, 3, 2},
                CHSearchEdge{2, 3, 4, 3},
                CHSearchEdge{3, 4, 5, 4},
                CHSearchEdge{4, 5, 6, 5},
                CHSearchEdge{5, 6, 7, 4},
                CHSearchEdge{6, 7, 8, 3},
                CHSearchEdge{7, 8, 9, 2},
                CHSearchEdge{77, 9, 10, 1}
            }
        },
        GraphForEachEdgeTestsParameter{
            [](G& g){ TestBasicReverseGraph(g); },
            std::vector<G::Edge> {
                CHSearchEdge{0, 1, 2, 2},
                CHSearchEdge{1, 1, 3, 2},
                CHSearchEdge{2, 2, 6, 8},
                CHSearchEdge{3, 3, 4, 3},
                CHSearchEdge{4, 4, 3, 2},
                CHSearchEdge{5, 4, 5, 2},
                CHSearchEdge{6, 5, 4, 4},
                CHSearchEdge{7, 4, 6, 6},
                CHSearchEdge{8, 5, 6, 2},
                CHSearchEdge{9, 6, 5, 3},
                CHSearchEdge{10, 5, 3, 7}
            }
        },
        GraphForEachEdgeTestsParameter{
            [](G& g){ TestBasicContractedGraph(g); },
            std::vector<G::Edge> {
                CHSearchEdge{0, 1, 2, 2},
                CHSearchEdge{1, 1, 3, 2},
                CHSearchEdge{2, 1, 4, 5, 3},
                CHSearchEdge{3, 2, 6, 8},
                CHSearchEdge{4, 2, 5, 11, 6},
                CHSearchEdge{5, 2, 4, 15, 5},
                CHSearchEdge{6, 3, 4, 3},
                CHSearchEdge{7, 4, 3, 2},
                CHSearchEdge{8, 4, 5, 2},
                CHSearchEdge{9, 4, 6, 6},
                CHSearchEdge{10, 5, 4, 4},
                CHSearchEdge{11, 5, 3, 7},
                CHSearchEdge{12, 5, 6, 2},
                CHSearchEdge{13, 6, 5, 3}
            }
        },
        GraphForEachEdgeTestsParameter{
            [](G& g){ TestBidirectedGraph(g); },
            std::vector<G::Edge> {
                typename G::Edge{0, 1, 2, 2, G::Edge::EdgeType::forward},
                typename G::Edge{1, 1, 3, 3, G::Edge::EdgeType::twoway},
                typename G::Edge{1, 3, 1, 3, G::Edge::EdgeType::twoway},
                typename G::Edge{2, 2, 6, 12, G::Edge::EdgeType::forward},
                typename G::Edge{3, 3, 4, 2, G::Edge::EdgeType::twoway},
                typename G::Edge{3, 4, 3, 2, G::Edge::EdgeType::twoway},
                typename G::Edge{4, 4, 5, 3, G::Edge::EdgeType::twoway},
                typename G::Edge{4, 5, 4, 3, G::Edge::EdgeType::twoway},
                typename G::Edge{5, 4, 6, 6, G::Edge::EdgeType::forward},
                typename G::Edge{6, 5, 6, 2, G::Edge::EdgeType::forward},
                typename G::Edge{7, 6, 5, 3, G::Edge::EdgeType::forward}
            }
        }
    )
);

TEST_P(GraphForEachEdgeTests, ForEachEdgeTest) {
    auto&& parameters = GetParam();
    parameters.function(g_);
    std::vector<G::Edge> actual_edges{};
    g_.ForEachEdge([&](G::Edge& edge) {
        actual_edges.push_back(edge);
        edge.Print();
    });

    EXPECT_THAT(actual_edges, testing::UnorderedElementsAreArray(parameters.expected_edges));
}

TEST(GraphTests, ForEachVertexBasicGraphTest) {
    G g;
    TestBasicContractedGraph(g);
    std::vector<G::Vertex> actual_vertices{};
    g.ForEachVertex([&](G::Vertex& vertex) {
        actual_vertices.push_back(vertex);
    });

    std::vector<G::Vertex> expected_vertices{
        Vertex{1, VectorEdgeRange<CHSearchEdge>{}},
        Vertex{2, VectorEdgeRange<CHSearchEdge>{}},
        Vertex{3, VectorEdgeRange<CHSearchEdge>{}},
        Vertex{4, VectorEdgeRange<CHSearchEdge>{}},
        Vertex{5, VectorEdgeRange<CHSearchEdge>{}},
        Vertex{6, VectorEdgeRange<CHSearchEdge>{}}
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