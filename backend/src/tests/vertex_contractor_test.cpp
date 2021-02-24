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
#include "routing/preprocessing/vertex_contractor.h"
#include <string>
#include <vector>
#include <tuple>

using namespace std;
using namespace routing;
using namespace database;
using namespace preprocessing;

using G = Graph<ContractionVertex<BasicEdge>, BasicEdge>;

class VertexContractorTest : public testing::TestWithParam<std::tuple<size_t, std::vector<BasicEdge>>> {
    protected:
    
    G g_;
    void SetUp() override {
        TestBasicReverseGraph(g_);
    }
};

INSTANTIATE_TEST_CASE_P(
    SimpleContractionParameters, 
    VertexContractorTest,
    ::testing::Values(
        std::make_tuple(3, std::vector<BasicEdge> { BasicEdge{1, 3, 4, 3}, BasicEdge{1, 3, 5, 5}}),
        std::make_tuple(5, std::vector<BasicEdge> { BasicEdge{1, 5, 4, 3}, BasicEdge{1, 5, 6, 2}, BasicEdge{1, 5, 3, 6}, BasicEdge{1, 5, 3, 5}}),
        std::make_tuple(6, std::vector<BasicEdge> { BasicEdge{1, 6, 5, 3}}),
        std::make_tuple(2, std::vector<BasicEdge> { BasicEdge{1, 2, 6, 8}}),
        std::make_tuple(1, std::vector<BasicEdge> { BasicEdge{1, 1, 2, 2}, BasicEdge{1, 1, 3, 2}})
    )
);


TEST_P(VertexContractorTest, SimpleContraction) {
    size_t tested_vertex_id = std::get<0>(GetParam());
    std::vector<BasicEdge> expected = std::get<1>(GetParam());

    ContractionVertex<BasicEdge>* tested_vertex = g_.GetVertex(tested_vertex_id);
    tested_vertex->ForEachEdge([](BasicEdge& e){ e.Print(); });
    VertexContractor<G> contractor{g_, 11};

    ContractionVertex<BasicEdge>* contracted_vertex = g_.GetVertex(4);
    ContractionVertex<BasicEdge>& v = *contracted_vertex;
   
    std::cout << "After contraction." << std::endl;
    contractor.ContractVertex(v);
    
    tested_vertex = g_.GetVertex(tested_vertex_id);
    tested_vertex->ForEachEdge([](BasicEdge& e){ e.Print(); });

    EXPECT_THAT(tested_vertex->get_edges(), testing::ElementsAreArray(expected));

}