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
#include "routing/preprocessing/vertex_measures.h"
#include "routing/preprocessing/graph_contractor.h"
#include "routing/preprocessing/contraction_parameters.h"

#include <string>
#include <vector>
#include <tuple>
#include <string_view>

using namespace std;
using namespace routing;
using namespace database;
using namespace preprocessing;

using G = Graph<ContractionVertex<BasicEdge>, BasicEdge>;

struct VertexMeasuresTest {
    int32_t edge_difference;
    int32_t deleted_neighbours;
    double contraction_priority;

    VertexMeasuresTest(int32_t ed, int32_t dn, double cp) 
        : edge_difference(ed), deleted_neighbours(dn), contraction_priority(cp) {}

    bool operator == (const VertexMeasuresTest& other) {
        if (edge_difference != other.edge_difference) {
            return false;
        }

        if (deleted_neighbours != other.deleted_neighbours) {
            return false;
        }

        return contraction_priority == other.contraction_priority;
    }
};

struct VertexMeasuresTestParameters {
    VertexMeasuresTest measures;
    unsigned_id_type tested_vertex;
    std::vector<unsigned_id_type> vertices_to_contract;

    VertexMeasuresTestParameters(VertexMeasuresTest m, unsigned_id_type v, const std::vector<unsigned_id_type>& vertices)
        : measures(m), tested_vertex(v), vertices_to_contract(vertices) {}
};

class VertexContractionMeasuresTests : public testing::TestWithParam<VertexMeasuresTestParameters> {
    protected:
    
    G g_;
    void SetUp() override {
        TestBasicReverseGraph(g_);
    }
};

INSTANTIATE_TEST_CASE_P(
    SimpleContractionEdgesTestParameters, 
    VertexContractionMeasuresTests,
    ::testing::Values(
        VertexMeasuresTestParameters{VertexMeasuresTest{-2, 0, -2}, 1, std::vector<unsigned_id_type>{}},
        VertexMeasuresTestParameters{VertexMeasuresTest{-2, 0, -2}, 2, std::vector<unsigned_id_type>{}},
        VertexMeasuresTestParameters{VertexMeasuresTest{-3, 0, -3}, 3, std::vector<unsigned_id_type>{}},
        VertexMeasuresTestParameters{VertexMeasuresTest{-2, 0, -2}, 4, std::vector<unsigned_id_type>{}},
        VertexMeasuresTestParameters{VertexMeasuresTest{-2, 0, -2}, 5, std::vector<unsigned_id_type>{}},
        VertexMeasuresTestParameters{VertexMeasuresTest{-3, 0, -3}, 6, std::vector<unsigned_id_type>{}},
        VertexMeasuresTestParameters{VertexMeasuresTest{-2, 0, -2}, 1, std::vector<unsigned_id_type>{ 6 }},
        VertexMeasuresTestParameters{VertexMeasuresTest{-2, 1, -1}, 2, std::vector<unsigned_id_type>{ 6 }},
        VertexMeasuresTestParameters{VertexMeasuresTest{-3, 0, -3}, 3, std::vector<unsigned_id_type>{ 6 }},
        VertexMeasuresTestParameters{VertexMeasuresTest{-2, 1, -1}, 4, std::vector<unsigned_id_type>{ 6 }},
        VertexMeasuresTestParameters{VertexMeasuresTest{-2, 1, -1}, 5, std::vector<unsigned_id_type>{ 6 }},
        VertexMeasuresTestParameters{VertexMeasuresTest{-2, 1, -1}, 1, std::vector<unsigned_id_type>{ 6, 3 }},
        VertexMeasuresTestParameters{VertexMeasuresTest{-2, 1, -1}, 2, std::vector<unsigned_id_type>{ 6, 3 }},
        VertexMeasuresTestParameters{VertexMeasuresTest{-2, 2, 0}, 4, std::vector<unsigned_id_type>{ 6, 3 }},
        VertexMeasuresTestParameters{VertexMeasuresTest{-2, 2, 0}, 5, std::vector<unsigned_id_type>{ 6, 3 }},
        VertexMeasuresTestParameters{VertexMeasuresTest{-4, 1, -3}, 3, std::vector<unsigned_id_type>{ 4 }},
        VertexMeasuresTestParameters{VertexMeasuresTest{-2, 1, -1}, 5, std::vector<unsigned_id_type>{ 4 }}, // DN of one edge + reverse edge.
        VertexMeasuresTestParameters{VertexMeasuresTest{-4, 2, -2}, 3, std::vector<unsigned_id_type>{ 4, 5 }}, // DN for two reverse edges.
        VertexMeasuresTestParameters{VertexMeasuresTest{-2, 2, 0}, 5, std::vector<unsigned_id_type>{ 4, 3 }} // DN for two edges.
        
        
    )
);

TEST_P(VertexContractionMeasuresTests, VertexContractionMeasuresTest) {
    VertexMeasuresTestParameters parameters = GetParam();
    GraphContractor<G> contractor{g_, ContractionParameters{11}};
    VertexMeasures measures{g_, ContractionParameters{11}};
    
    for(auto&& v : parameters.vertices_to_contract) {
        contractor.ContractVertex(*(g_.GetVertex(v)));
    }
    auto&& tested_vertex = g_.GetVertex(parameters.tested_vertex);
    double edge_difference = measures.CalculateEdgeDifference(*tested_vertex);
    int32_t deleted_neighbours = measures.CalculateDeletedNeighbours(*tested_vertex);
    int32_t contraction_priority = measures.CalculateContractionAttractivity(*tested_vertex);
    std::cout << "Test measures of vertex " << parameters.tested_vertex << std::endl;
    std::cout << "Edge difference: expected: " << parameters.measures.edge_difference << " actual: " << edge_difference << std::endl;
    std::cout << "Deleted neighbours: expected: " << parameters.measures.deleted_neighbours << " actual: " << deleted_neighbours << std::endl;
    std::cout << "Contraction priority: expected: " << parameters.measures.contraction_priority << " actual: " << contraction_priority << std::endl;
    EXPECT_EQ(parameters.measures.edge_difference, edge_difference);
    EXPECT_EQ(parameters.measures.deleted_neighbours, deleted_neighbours);
    EXPECT_EQ(parameters.measures.contraction_priority, contraction_priority);
}