#include "gtest/gtest.h"
#include "gmock/gmock.h"  
#include "routing/graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/edges/ch_preprocessing_edge.h"
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

using G = Graph<ContractionVertex<CHPreprocessingEdge>, CHPreprocessingEdge>;
struct ExpectedVertexProperties;
std::vector<ExpectedVertexProperties> QueueToVector(GraphContractor<G>::PriorityQueue& q);
void Print(std::vector<ExpectedVertexProperties>& v, std::string_view name); 

struct ExpectedVertexProperties {
    unsigned_id_type vertex_id_;
    double contraction_priority_;

    bool operator ==(const ExpectedVertexProperties& other) const {
        if (vertex_id_ != other.vertex_id_) {
            return false;
        }

        return contraction_priority_ == other.contraction_priority_;
    }

    bool operator !=(const ExpectedVertexProperties& other) const {
        return !(*this == other);
    }

    void Print() {
        std::cout << "Vertex id: " << vertex_id_ << ", Contraction priority: " << contraction_priority_ << std::endl;
    }

    ExpectedVertexProperties(unsigned_id_type vertex_id, double contraction_priority) 
        : vertex_id_(vertex_id), contraction_priority_(contraction_priority) {}
};

class GraphContractorLazyUpdateTests : public testing::Test {
    protected:
    
    G g_;
    void SetUp() override {
        TestBasicReverseGraph(g_);
    }
};

TEST_F(GraphContractorLazyUpdateTests, GraphContractorLazyUpdateTest) {
    GraphContractor<G> contractor{g_, ContractionParameters{11}};
    std::vector<ExpectedVertexProperties> expected{ ExpectedVertexProperties{2, -2} };
    GraphContractor<G>::PriorityQueue q{};
    q.emplace(-4, 2);
    q.emplace(-3, 3);
    contractor.ContractMinVertex(q);
    auto&& actual = QueueToVector(q);
    Print(actual, "actual");
    Print(expected, "expected");
    EXPECT_EQ(false, g_.GetVertex(2).IsContracted());
    EXPECT_EQ(true, g_.GetVertex(3).IsContracted());
    EXPECT_THAT(actual, testing::UnorderedElementsAreArray(expected));

}

std::vector<ExpectedVertexProperties> QueueToVector(GraphContractor<G>::PriorityQueue& q) {
    std::vector<ExpectedVertexProperties> v{};
    while(!q.empty()) {
        auto&& pair = q.top();
        v.emplace_back(pair.second, pair.first);
        q.pop();
    }
    return v;
}

void Print(std::vector<ExpectedVertexProperties>& v, std::string_view name) {
    std::cout << name << ":" << std::endl;
    for(auto&& e : v) {
        e.Print();
    }
}