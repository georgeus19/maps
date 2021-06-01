#include "gtest/gtest.h"
#include "gmock/gmock.h"  
#include "routing/adjacency_list_graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/edges/ch_edge.h"
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
#include "routing/edges/length_source.h"

#include <string>
#include <vector>

using namespace std;
using namespace routing;
using namespace database;
using namespace query;
using namespace preprocessing;
// using namespace testing;
using Edge = CHEdge<NumberLengthSource>;
using Vertex = CHVertex<Edge, VectorEdgeRange<Edge>>;
using G = AdjacencyListGraph<Vertex, Edge>;

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
                Edge{0, 1, 2, 1},
                Edge{1, 2, 3, 2},
                Edge{2, 3, 4, 3},
                Edge{3, 4, 5, 4},
                Edge{4, 5, 6, 5},
                Edge{5, 6, 7, 4},
                Edge{6, 7, 8, 3},
                Edge{7, 8, 9, 2},
                Edge{77, 9, 10, 1}
            }
        },
        GraphForEachEdgeTestsParameter{
            [](G& g){ TestBasicReverseGraph(g); },
            std::vector<G::Edge> {
                Edge{0, 1, 2, 2},
                Edge{1, 1, 3, 2},
                Edge{2, 2, 6, 8},
                Edge{3, 3, 4, 3},
                Edge{4, 4, 3, 2},
                Edge{5, 4, 5, 2},
                Edge{6, 5, 4, 4},
                Edge{7, 4, 6, 6},
                Edge{8, 5, 6, 2},
                Edge{9, 6, 5, 3},
                Edge{10, 5, 3, 7}
            }
        },
        GraphForEachEdgeTestsParameter{
            [](G& g){ TestBasicContractedGraph(g); },
            std::vector<G::Edge> {
                Edge{0, 1, 2, 2},
                Edge{1, 1, 3, 2},
                Edge{2, 1, 4, 5, 3},
                Edge{3, 2, 6, 8},
                Edge{4, 2, 5, 11, 6},
                Edge{5, 2, 4, 15, 5},
                Edge{6, 3, 4, 3},
                Edge{7, 4, 3, 2},
                Edge{8, 4, 5, 2},
                Edge{9, 4, 6, 6},
                Edge{10, 5, 4, 4},
                Edge{11, 5, 3, 7},
                Edge{12, 5, 6, 2},
                Edge{13, 6, 5, 3}
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
        Vertex{1, VectorEdgeRange<Edge>{}},
        Vertex{2, VectorEdgeRange<Edge>{}},
        Vertex{3, VectorEdgeRange<Edge>{}},
        Vertex{4, VectorEdgeRange<Edge>{}},
        Vertex{5, VectorEdgeRange<Edge>{}},
        Vertex{6, VectorEdgeRange<Edge>{}}
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