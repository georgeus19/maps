#include "gtest/gtest.h"
#include "gmock/gmock.h"  
#include "routing/adjacency_list_graph.h"
#include "routing/edges/basic_edge.h"

#include "routing/algorithm.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/query/dijkstra.h"
#include "routing/exception.h"
#include "routing/database/database_helper.h"
#include "routing/utility/point.h"
#include "routing/bidirectional_graph.h"
#include "tests/graph_test.h"
#include "routing/preprocessing/vertex_measures.h"
#include "routing/preprocessing/graph_contractor.h"
#include "routing/vertices/ch_vertex.h"
#include "routing/edge_ranges/vector_edge_range.h"
#include "routing/edges/length_source.h"

#include <string>
#include <vector>
#include <tuple>

using namespace std;
using namespace routing;
using namespace query;
using namespace database;
using namespace preprocessing;

using Edge = CHEdge<NumberLengthSource>;
using G = BidirectionalGraph<AdjacencyListGraph<CHVertex<Edge, VectorEdgeRange<Edge>>, Edge>>;
void ContractVertex(G& g, GraphContractor<G> & contractor, size_t id);

class GraphContractorSimpleEdgesTests : public testing::TestWithParam<std::tuple<size_t, std::vector<Edge>>> {
    protected:
    
    G g_;
    void SetUp() override {
        TestBasicReverseGraph(g_);
    }
};

INSTANTIATE_TEST_CASE_P(
    SimpleEdgesTestParameters, 
    GraphContractorSimpleEdgesTests,
    ::testing::Values(
        std::make_tuple(1, std::vector<Edge> { Edge{1, 1, 2, 2}, Edge{1, 1, 3, 2}}),
        std::make_tuple(2, std::vector<Edge> { Edge{1, 2, 6, 8}}),
        std::make_tuple(3, std::vector<Edge> { Edge{1, 3, 4, 3}, Edge{1, 3, 5, 5}, Edge{1, 3, 6, 9}}),
        std::make_tuple(4, std::vector<Edge> { Edge{1, 4, 3, 2}, Edge{1, 4, 5, 2}, Edge{1, 4, 6, 6}}),
        std::make_tuple(5, std::vector<Edge> { Edge{1, 5, 4, 4}, Edge{1, 5, 3, 7}, Edge{1, 5, 6, 2}, Edge{1, 5, 3, 6}}),
        std::make_tuple(6, std::vector<Edge> { Edge{1, 6, 5, 3}})
    )
);

TEST_P(GraphContractorSimpleEdgesTests, SimpleEdgesTest) {
    size_t tested_vertex_id = std::get<0>(GetParam());
    std::vector<Edge> expected = std::get<1>(GetParam());

    CHVertex<Edge, VectorEdgeRange<Edge>> tested_vertex = g_.GetVertex(tested_vertex_id);
    tested_vertex.ForEachEdge([](Edge& e){ e.Print(); });
    GraphContractor<G> contractor{g_, ContractionParameters{5, 1, 1, 0}, 11};
   
    ContractVertex(g_, contractor, 4);
    std::cout << "After contraction." << std::endl;
    
    tested_vertex = g_.GetVertex(tested_vertex_id);
    std::vector<Edge> actual{};
    tested_vertex.ForEachEdge([&](Edge& e){ 
        e.Print();
        actual.push_back(e);
     });
    
    EXPECT_THAT(actual, testing::UnorderedElementsAreArray(expected));
}

class GraphContractorSimpleBackwardEdgesTests : public testing::TestWithParam<std::tuple<size_t, std::vector<Edge>>> {
    protected:
    
    G g_;
    void SetUp() override {
        TestBasicReverseGraph(g_);
    }
};

INSTANTIATE_TEST_CASE_P(
    SimpleContractionBackwardEdgesTestParameters, 
    GraphContractorSimpleBackwardEdgesTests,
    ::testing::Values(
        std::make_tuple(3, std::vector<Edge> { 
            Edge{1, 3, 1, 2, Edge::EdgeType::backward},
            Edge{1, 3, 4, 2, Edge::EdgeType::backward},
            Edge{1, 3, 5, 7, Edge::EdgeType::backward},
            Edge{1, 3, 5, 6, Edge::EdgeType::backward}
        }),
        std::make_tuple(5, std::vector<Edge> { 
            Edge{1, 5, 4, 2, Edge::EdgeType::backward},
            Edge{1, 5, 6, 3, Edge::EdgeType::backward},
            Edge{1, 5, 3, 5, Edge::EdgeType::backward}
        }),
        std::make_tuple(6, std::vector<Edge> { 
            Edge{1, 6, 2, 8, Edge::EdgeType::backward},
            Edge{1, 6, 4, 6, Edge::EdgeType::backward},
            Edge{1, 6, 5, 2, Edge::EdgeType::backward},
            Edge{1, 6, 3, 9, Edge::EdgeType::backward}
        }),
        std::make_tuple(2, std::vector<Edge> { 
            Edge{1, 2, 1, 2, Edge::EdgeType::backward}
        }),
        std::make_tuple(1, std::vector<Edge> {})
    )
);

TEST_P(GraphContractorSimpleBackwardEdgesTests, SimpleContractionBackwardEdgesTest) {
    size_t tested_vertex_id = std::get<0>(GetParam());
    std::vector<Edge> expected = std::get<1>(GetParam());

    CHVertex<Edge, VectorEdgeRange<Edge>> tested_vertex = g_.GetVertex(tested_vertex_id);
    tested_vertex.ForEachBackwardEdge([](Edge& e){ e.Print(); });
    GraphContractor<G> contractor{g_, ContractionParameters{5, 1, 1, 0}, 11};

    ContractVertex(g_, contractor, 4);
   
    std::cout << "After contraction." << std::endl;
    
    tested_vertex = g_.GetVertex(tested_vertex_id);
    std::vector<Edge> actual{};
    tested_vertex.ForEachBackwardEdge([&](Edge& e){ 
        e.Print();
        actual.push_back(e);
    });

    EXPECT_THAT(actual, testing::UnorderedElementsAreArray(expected));
}

class GraphContractorDoubleContractionEdgesTests : public testing::TestWithParam<std::tuple<size_t, std::vector<Edge>>> {
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
        std::make_tuple(1, std::vector<Edge> { Edge{1, 1, 2, 2}, Edge{1, 1, 3, 2}}),
        std::make_tuple(2, std::vector<Edge> { Edge{1, 2, 6, 8}}),
        std::make_tuple(3, std::vector<Edge> { Edge{1, 3, 4, 3}, Edge{1, 3, 5, 5}, Edge{1, 3, 6, 9}, Edge{1, 3, 6, 7}}),
        std::make_tuple(4, std::vector<Edge> { Edge{1, 4, 3, 2}, Edge{1, 4, 5, 2}, Edge{1, 4, 6, 6}}),
        std::make_tuple(5, std::vector<Edge> { Edge{1, 5, 4, 4}, Edge{1, 5, 6, 2}, Edge{1, 5, 3, 7}, Edge{1, 5, 3, 6}}),
        std::make_tuple(6, std::vector<Edge> { Edge{1, 6, 5, 3}, Edge{1, 6, 3, 9}})
    )
);

TEST_P(GraphContractorDoubleContractionEdgesTests, DoubleContractionEdgesTest) {
    size_t tested_vertex_id = std::get<0>(GetParam());
    std::cout << "Double contraction - tested vertex is " << tested_vertex_id << std::endl;
    std::vector<Edge> expected = std::get<1>(GetParam());

    CHVertex<Edge, VectorEdgeRange<Edge>> tested_vertex = g_.GetVertex(tested_vertex_id);
    tested_vertex.ForEachEdge([](Edge& e){ e.Print(); });
    GraphContractor<G> contractor{g_, ContractionParameters{5, 1, 1, 0}, 11};

    ContractVertex(g_, contractor, 4);
    std::cout << "After first contraction." << std::endl;
    tested_vertex = g_.GetVertex(tested_vertex_id);
    tested_vertex.ForEachEdge([](Edge& e){ e.Print(); });
    ContractVertex(g_, contractor, 5);
    std::cout << "After second  contraction." << std::endl;
    
    tested_vertex = g_.GetVertex(tested_vertex_id);
    std::vector<Edge> actual{};
    tested_vertex.ForEachEdge([&](Edge& e){ 
        actual.push_back(e);
        e.Print();
    });
    EXPECT_THAT(actual, testing::UnorderedElementsAreArray(expected));
}

void ContractVertex(G& g, GraphContractor<G> & contractor, size_t id) {
    contractor.ContractVertex(g.GetVertex(id));
}

TEST(GraphContractorContractionPriority, BasicGraphPriority) {
    G g{};
    TestBasicReverseGraph(g);

    std::vector<std::pair<double, unsigned_id_type>> expected_priorities {
        std::make_pair(-2, 1),
        std::make_pair(-2, 2),
        std::make_pair(-3, 3),
        std::make_pair(-2, 4),
        std::make_pair(-2, 5),
        std::make_pair(-3, 6)
    };

    GraphContractor<G> contractor(g,  ContractionParameters{5, 1, 1, 0}, 11);
    auto&& q = contractor.CalculateContractionPriority();

    std::vector<std::pair<double, unsigned_id_type>> actual_priorities{};
    while(!q.empty()) {
        auto pair = q.top();
        actual_priorities.push_back(pair);
        q.pop();
    }
    EXPECT_THAT(actual_priorities, testing::UnorderedElementsAreArray(expected_priorities));

}

class GraphContractorTwowayEdgesTests : public testing::TestWithParam<std::tuple<size_t, std::vector<Edge>>> {
    protected:
    
    G g_;
    void SetUp() override {
        TestBidirectedGraph(g_);
    }
};

INSTANTIATE_TEST_CASE_P(
    TwowayEdgesTestParameters, 
    GraphContractorTwowayEdgesTests,
    ::testing::Values(
        std::make_tuple(1, std::vector<Edge> {
            Edge{1, 1, 2, 2, Edge::EdgeType::forward},
            Edge{1, 1, 3, 3, Edge::EdgeType::twoway}
        }),
        std::make_tuple(2, std::vector<Edge> {
            Edge{1, 2, 6, 12, Edge::EdgeType::forward}
        }),
        std::make_tuple(3, std::vector<Edge> {
            Edge{1, 3, 1, 3, Edge::EdgeType::twoway}, 
            Edge{1, 3, 4, 2, Edge::EdgeType::twoway},
            Edge{1, 3, 5, 5, Edge::EdgeType::twoway},
            Edge{1, 3, 6, 8, Edge::EdgeType::forward}
        }),
        std::make_tuple(4, std::vector<Edge> {
            Edge{1, 4, 3, 2, Edge::EdgeType::twoway},
            Edge{1, 4, 5, 3, Edge::EdgeType::twoway},
            Edge{1, 4, 6, 6, Edge::EdgeType::forward}
        }),
        std::make_tuple(5, std::vector<Edge> {
            Edge{1, 5, 4, 3, Edge::EdgeType::twoway},
            Edge{1, 5, 6, 2, Edge::EdgeType::forward},
            Edge{1, 5, 3, 5, Edge::EdgeType::twoway}
        }),
        std::make_tuple(6, std::vector<Edge> {
            Edge{1, 6, 5, 3, Edge::EdgeType::forward}
        })
    )
);

TEST_P(GraphContractorTwowayEdgesTests, TwowayEdgesTest) {
    size_t tested_vertex_id = std::get<0>(GetParam());
    std::vector<Edge> expected = std::get<1>(GetParam());

    CHVertex<Edge, VectorEdgeRange<Edge>> tested_vertex = g_.GetVertex(tested_vertex_id);
    tested_vertex.ForEachEdge([](Edge& e){ e.Print(); });
    GraphContractor<G> contractor{g_, ContractionParameters{5, 1, 1, 0}, 11};
   
    contractor.ContractVertex(g_.GetVertex(4));
    std::cout << "After contraction." << std::endl;
    
    tested_vertex = g_.GetVertex(tested_vertex_id);
    std::vector<Edge> actual{};
    tested_vertex.ForEachEdge([&](Edge& e){ 
        e.Print();
        actual.push_back(e);
     });
    
    EXPECT_THAT(actual, testing::ElementsAreArray(expected));
}

class GraphContractorTwowayBakwardEdgesTests : public testing::TestWithParam<std::tuple<size_t, std::vector<Edge>>> {
    protected:
    
    G g_;
    void SetUp() override {
        TestBidirectedGraph(g_);
    }
};

INSTANTIATE_TEST_CASE_P(
    TwowayBackwardEdgesTestParameters, 
    GraphContractorTwowayBakwardEdgesTests,
    ::testing::Values(
        std::make_tuple(1, std::vector<Edge> {
            Edge{1, 1, 3, 3, Edge::EdgeType::twoway}
        }),
        std::make_tuple(2, std::vector<Edge> {
            Edge{2, 2, 1, 2, Edge::EdgeType::backward}
        }),
        std::make_tuple(3, std::vector<Edge> {
            Edge{3, 3, 1, 3, Edge::EdgeType::twoway}, 
            Edge{4, 3, 4, 2, Edge::EdgeType::twoway},
            Edge{5, 3, 5, 5, Edge::EdgeType::twoway}
        }),
        std::make_tuple(4, std::vector<Edge> {
            Edge{6, 4, 3, 2, Edge::EdgeType::twoway},
            Edge{7, 4, 5, 3, Edge::EdgeType::twoway}
        }),
        std::make_tuple(5, std::vector<Edge> {
            Edge{8, 5, 4, 3, Edge::EdgeType::twoway},
            Edge{10, 5, 6, 3, Edge::EdgeType::backward},
            Edge{9, 5, 3, 5, Edge::EdgeType::twoway}
        }),
        std::make_tuple(6, std::vector<Edge> {
            Edge{11, 6, 2, 12, Edge::EdgeType::backward},
            Edge{13, 6, 4, 6, Edge::EdgeType::backward},
            Edge{14, 6, 5, 2, Edge::EdgeType::backward},
            Edge{12, 6, 3, 8, Edge::EdgeType::backward}
        })
    )
);

TEST_P(GraphContractorTwowayBakwardEdgesTests, TwowayBackwardEdgesTest) {
    size_t tested_vertex_id = std::get<0>(GetParam());
    std::vector<Edge> expected = std::get<1>(GetParam());

    CHVertex<Edge, VectorEdgeRange<Edge>> tested_vertex = g_.GetVertex(tested_vertex_id);
    tested_vertex.ForEachBackwardEdge([](Edge& e){ e.Print(); });
    GraphContractor<G> contractor{g_, ContractionParameters{5, 1, 1, 0}, 11};

    ContractVertex(g_, contractor, 4);
   
    std::cout << "After contraction." << std::endl;
    
    tested_vertex = g_.GetVertex(tested_vertex_id);
    std::vector<Edge> actual{};
    tested_vertex.ForEachBackwardEdge([&](Edge& e){ 
        e.Print();
        actual.push_back(e);
    });

    EXPECT_THAT(actual, testing::ElementsAreArray(expected));
}