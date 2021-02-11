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
using namespace std;
using namespace routing;
using namespace database;
using namespace preprocessing;

using G = Graph<ContractionVertex<BasicEdge>, BasicEdge>;

class VertexContractorTest : public testing::Test {
    protected:
    
    G g_;
    void SetUp() override {
        TestBasicGraph(g_);
    }
};


TEST_F(VertexContractorTest, SimpleContraction) {
    VertexContractor<G> contractor{g_, 11};

    ContractionVertex<BasicEdge>* contracted_vertex = g_.GetVertex(4);
    ContractionVertex<BasicEdge>& v = *contracted_vertex;
    contractor.ContractVertex(v);
    std::vector<BasicEdge> three_edges {
        BasicEdge{1, 3, 4, 3}, BasicEdge{1, 3, 5, 5}
    };
    ContractionVertex<BasicEdge>* vertex_three = g_.GetVertex(3);
    vertex_three->ForEachEdge([](BasicEdge& e){
        std::cout << e.get_from() << "->" << e.get_to() << std::endl;
    });

    EXPECT_THAT(vertex_three->get_edges(), testing::ElementsAreArray(three_edges));

}