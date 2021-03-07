#include "gtest/gtest.h"
#include "gmock/gmock.h"  
#include "routing/graph.h"
#include "routing/edge.h"
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
#include <string>
#include <vector>
#include <tuple>

using namespace std;
using namespace routing;
using namespace database;
using namespace preprocessing;

using G = Graph<ContractionVertex<BasicEdge>, BasicEdge>;
void ContractVertex(G& g, GraphContractor<G> & contractor, size_t id);

class GraphContractorSimpleEdgesTests : public testing::TestWithParam<std::tuple<size_t, std::vector<BasicEdge>>> {
    protected:
    
    G g_;
    void SetUp() override {
        TestBasicReverseGraph(g_);
    }
};

INSTANTIATE_TEST_CASE_P(
    SimpleContractionEdgesTestParameters, 
    GraphContractorSimpleEdgesTests,
    ::testing::Values(
        std::make_tuple(1, std::vector<BasicEdge> { BasicEdge{1, 1, 2, 2}, BasicEdge{1, 1, 3, 2}}),
        std::make_tuple(2, std::vector<BasicEdge> { BasicEdge{1, 2, 6, 8}}),
        std::make_tuple(3, std::vector<BasicEdge> { BasicEdge{1, 3, 4, 3}, BasicEdge{1, 3, 5, 5}, BasicEdge{1, 3, 6, 9}}),
        std::make_tuple(4, std::vector<BasicEdge> { BasicEdge{1, 4, 3, 2}, BasicEdge{1, 4, 5, 2}, BasicEdge{1, 4, 6, 6}}),
        std::make_tuple(5, std::vector<BasicEdge> { BasicEdge{1, 5, 4, 4}, BasicEdge{1, 5, 6, 2}, BasicEdge{1, 5, 3, 7}, BasicEdge{1, 5, 3, 6}}),
        std::make_tuple(6, std::vector<BasicEdge> { BasicEdge{1, 6, 5, 3}})
    )
);

class GraphContractorDoubleContractionEdgesTests : public testing::TestWithParam<std::tuple<size_t, std::vector<BasicEdge>>> {
    protected:
    
    G g_;
    void SetUp() override {
        TestBasicReverseGraph(g_);
    }
};

INSTANTIATE_TEST_CASE_P(
    DoubleContractionEdgesTestParameters, 
    GraphContractorDoubleContractionEdgesTests,
    ::testing::Values(
        std::make_tuple(1, std::vector<BasicEdge> { BasicEdge{1, 1, 2, 2}, BasicEdge{1, 1, 3, 2}}),
        std::make_tuple(2, std::vector<BasicEdge> { BasicEdge{1, 2, 6, 8}}),
        std::make_tuple(3, std::vector<BasicEdge> { BasicEdge{1, 3, 4, 3}, BasicEdge{1, 3, 5, 5}, BasicEdge{1, 3, 6, 9}, BasicEdge{1, 3, 6, 7}}),
        std::make_tuple(4, std::vector<BasicEdge> { BasicEdge{1, 4, 3, 2}, BasicEdge{1, 4, 5, 2}, BasicEdge{1, 4, 6, 6}}),
        std::make_tuple(5, std::vector<BasicEdge> { BasicEdge{1, 5, 4, 4}, BasicEdge{1, 5, 6, 2}, BasicEdge{1, 5, 3, 7}, BasicEdge{1, 5, 3, 6}}),
        std::make_tuple(6, std::vector<BasicEdge> { BasicEdge{1, 6, 5, 3}, BasicEdge{1, 6, 3, 10}, BasicEdge{1, 6, 3, 9}})
    )
);

class GraphContractorSimpleReverseEdgesTests : public testing::TestWithParam<std::tuple<size_t, std::vector<BasicEdge>>> {
    protected:
    
    G g_;
    void SetUp() override {
        TestBasicReverseGraph(g_);
    }
};

INSTANTIATE_TEST_CASE_P(
    SimpleContractionReverseEdgesTestParameters, 
    GraphContractorSimpleReverseEdgesTests,
    ::testing::Values(
        std::make_tuple(3, std::vector<BasicEdge> { BasicEdge{1, 3, 1, 2}, BasicEdge{1, 3, 4, 2}, BasicEdge{1, 3, 5, 7}, BasicEdge{1, 3, 5, 6}}),
        std::make_tuple(5, std::vector<BasicEdge> { BasicEdge{1, 5, 4, 2}, BasicEdge{1, 5, 6, 3}, BasicEdge{1, 5, 3, 5}}),
        std::make_tuple(6, std::vector<BasicEdge> { BasicEdge{1, 6, 2, 8}, BasicEdge{1, 6, 4, 6}, BasicEdge{1, 6, 5, 2}, BasicEdge{1, 6, 3, 9}}),
        std::make_tuple(2, std::vector<BasicEdge> { BasicEdge{1, 2, 1, 2}}),
        std::make_tuple(1, std::vector<BasicEdge> {})
    )
);


TEST_P(GraphContractorSimpleEdgesTests, SimpleContractionEdgesTest) {
    size_t tested_vertex_id = std::get<0>(GetParam());
    std::vector<BasicEdge> expected = std::get<1>(GetParam());

    ContractionVertex<BasicEdge>* tested_vertex = g_.GetVertex(tested_vertex_id);
    tested_vertex->ForEachEdge([](BasicEdge& e){ e.Print(); });
    GraphContractor<G> contractor{g_, ContractionParameters{11}};
   
    ContractVertex(g_, contractor, 4);
    std::cout << "After contraction." << std::endl;
    
    tested_vertex = g_.GetVertex(tested_vertex_id);
    tested_vertex->ForEachEdge([](BasicEdge& e){ e.Print(); });

    EXPECT_THAT(tested_vertex->get_edges(), testing::ElementsAreArray(expected));
}

TEST_P(GraphContractorSimpleReverseEdgesTests, SimpleContractionReverseEdgesTest) {
    size_t tested_vertex_id = std::get<0>(GetParam());
    std::vector<BasicEdge> expected = std::get<1>(GetParam());

    ContractionVertex<BasicEdge>* tested_vertex = g_.GetVertex(tested_vertex_id);
    tested_vertex->ForEachReverseEdge([](BasicEdge& e){ e.Print(); });
    GraphContractor<G> contractor{g_, ContractionParameters{11}};

    ContractVertex(g_, contractor, 4);
   
    std::cout << "After contraction." << std::endl;
    
    tested_vertex = g_.GetVertex(tested_vertex_id);
    tested_vertex->ForEachReverseEdge([](BasicEdge& e){ e.Print(); });

    EXPECT_THAT(tested_vertex->get_reverse_edges(), testing::ElementsAreArray(expected));
}

TEST_P(GraphContractorDoubleContractionEdgesTests, DoubleContractionEdgesTest) {
    size_t tested_vertex_id = std::get<0>(GetParam());
    std::cout << "Double contraction - tested vertex is " << tested_vertex_id << std::endl;
    std::vector<BasicEdge> expected = std::get<1>(GetParam());

    ContractionVertex<BasicEdge>* tested_vertex = g_.GetVertex(tested_vertex_id);
    tested_vertex->ForEachEdge([](BasicEdge& e){ e.Print(); });
    GraphContractor<G> contractor{g_, ContractionParameters{11}};

    ContractVertex(g_, contractor, 4);
    std::cout << "After first contraction." << std::endl;
    tested_vertex = g_.GetVertex(tested_vertex_id);
    tested_vertex->ForEachEdge([](BasicEdge& e){ e.Print(); });
    ContractVertex(g_, contractor, 5);
    std::cout << "After second  contraction." << std::endl;
    
    tested_vertex = g_.GetVertex(tested_vertex_id);
    tested_vertex->ForEachEdge([](BasicEdge& e){ e.Print(); });

    EXPECT_THAT(tested_vertex->get_edges(), testing::ElementsAreArray(expected));
}

void ContractVertex(G& g, GraphContractor<G> & contractor, size_t id) {
    ContractionVertex<BasicEdge>* contracted_vertex = g.GetVertex(id);
    // ContractionVertex<BasicEdge>& v = *contracted_vertex;
    contractor.ContractVertex(*contracted_vertex);
}

