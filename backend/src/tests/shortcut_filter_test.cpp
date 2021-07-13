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
#include "tests/graph_test.h"
#include "routing/preprocessing/vertex_measures.h"
#include "routing/preprocessing/graph_contractor.h"
#include "routing/bidirectional_graph.h"
#include "routing/preprocessing/contraction_parameters.h"
#include "routing/query/bidirectional_dijkstra.h"
#include "routing/preprocessing/shortcut_finder.h"
#include "routing/preprocessing/shortcut_filter.h"
#include "routing/vertices/ch_vertex.h"
#include "routing/edge_ranges/vector_edge_range.h"
#include "routing/edges/length_source.h"
#include "routing/types.h"

#include <string>
#include <vector>
#include <tuple>
#include <string_view>

using namespace std;
using namespace routing;
using namespace query;
using namespace database;
using namespace preprocessing;

using Edge = CHEdge<NumberLengthSource>;
using G = BidirectionalGraph<AdjacencyListGraph<CHVertex<Edge, VectorEdgeRange<Edge>>, Edge>>;
void Print(const std::vector<Edge>& edges, const std::string& name);

void Print(const std::vector<Edge>& edges, const std::string& name) {
    std::cout << name << " :" << std::endl;
    for(auto&& edge : edges) {
        edge.Print();
    }
}


struct FilterShortcutsParameter {
    std::vector<Edge> input_shortcuts;
    std::vector<Edge> expected_shortcuts;

    FilterShortcutsParameter(const std::vector<Edge>& is, const std::vector<Edge>& es) :
        input_shortcuts(is), expected_shortcuts(es) {}
};

class ShortcutFilterFilterShortcutsTests : public testing::TestWithParam<FilterShortcutsParameter> {
    protected:
    
    G g_;
    void SetUp() override {
        TestBasicReverseGraph(g_);
    }
};

INSTANTIATE_TEST_CASE_P(
    ShortcutFilterFilterShortcutsTestParameters, 
    ShortcutFilterFilterShortcutsTests,
    ::testing::Values(
        FilterShortcutsParameter{ 
            std::vector<Edge>{Edge{10, 1, 4, 1}},
            std::vector<Edge>{Edge{10, 1, 4, 1}}
        },
        FilterShortcutsParameter{ 
            std::vector<Edge>{Edge{10, 1, 3, 1}, Edge{11, 1, 4, 1}},
            std::vector<Edge>{Edge{10, 1, 3, 1}, Edge{11, 1, 4, 1}}
        },
        FilterShortcutsParameter{ 
            std::vector<Edge>{Edge{10, 1, 6, 1}, Edge{11, 1, 4, 1}, Edge{12, 1, 6, 2}},
            std::vector<Edge>{Edge{10, 1, 6, 1}, Edge{11, 1, 4, 1}}
        },
        FilterShortcutsParameter{ 
            std::vector<Edge>{Edge{10, 1, 3, 1}, Edge{11, 1, 4, 1}, Edge{12, 1, 3, 1}},
            std::vector<Edge>{Edge{10, 1, 3, 1}, Edge{11, 1, 4, 1}}
        },
        FilterShortcutsParameter{ 
            std::vector<Edge>{Edge{10, 1, 6, 4}, Edge{11, 1, 3, 2}, Edge{12, 1, 6, 3}},
            std::vector<Edge>{Edge{11, 1, 3, 2}, Edge{12, 1, 6, 3}}
        }
    )
);

TEST_P(ShortcutFilterFilterShortcutsTests, FilterShortcutsSimpleTest) {
    FilterShortcutsParameter param = GetParam();
    ShortcutFilter<G> filter{g_};
    auto&& actual_shortcuts = filter.FilterDuplicateShortcuts(param.input_shortcuts);
    Print(param.input_shortcuts, "Input shortcuts");
    Print(actual_shortcuts, "Actual shorctuts");
    Print(param.expected_shortcuts, "Expected shortcuts");

    EXPECT_THAT(actual_shortcuts, testing::ElementsAreArray(param.expected_shortcuts));
}


class ShortcutFilterTwowayEdgeMergeTests : public testing::TestWithParam<FilterShortcutsParameter> {
    protected:
    
    G g_;
    void SetUp() override {
        TestBasicReverseGraph(g_);
    }
};

INSTANTIATE_TEST_CASE_P(
    ShortcutFilterTwowayEdgeMergeTestParameters, 
    ShortcutFilterTwowayEdgeMergeTests,
    ::testing::Values(
        FilterShortcutsParameter{ 
            std::vector<Edge>{
                Edge{10, 1, 4, 1, Edge::EdgeType::forward},
                Edge{10, 4, 1, 1, Edge::EdgeType::forward}
            },
            std::vector<Edge>{Edge{10, 1, 4, 1, Edge::EdgeType::twoway}}
        },
        FilterShortcutsParameter{ 
            std::vector<Edge>{
                Edge{10, 1, 4, 3, Edge::EdgeType::forward},
                Edge{10, 4, 1, 1, Edge::EdgeType::forward}
            },
            std::vector<Edge>{
                Edge{10, 1, 4, 3, Edge::EdgeType::forward},
                Edge{10, 4, 1, 1, Edge::EdgeType::forward}
            }
        }
    )
);

TEST_P(ShortcutFilterTwowayEdgeMergeTests, MergeTwowayShortcuts) {
    FilterShortcutsParameter param = GetParam();
    ShortcutFilter<G> filter{g_};
    auto&& actual_shortcuts = filter.MergeUnorderedShortcuts(param.input_shortcuts);
    Print(param.input_shortcuts, "Input shortcuts");
    Print(actual_shortcuts, "Actual shorctuts");
    Print(param.expected_shortcuts, "Expected shortcuts");

    EXPECT_THAT(actual_shortcuts, testing::ElementsAreArray(param.expected_shortcuts));
}

