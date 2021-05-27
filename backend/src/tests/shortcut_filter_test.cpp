#include "gtest/gtest.h"
#include "gmock/gmock.h"  
#include "routing/adjacency_list_graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/edges/ch_preprocessing_edge.h"
#include "routing/algorithm.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/query/dijkstra.h"
#include "routing/exception.h"
#include "database/database_helper.h"
#include "utility/point.h"
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

#include <string>
#include <vector>
#include <tuple>
#include <string_view>

using namespace std;
using namespace routing;
using namespace query;
using namespace database;
using namespace preprocessing;

using G = BidirectionalGraph<AdjacencyListGraph<CHVertex<CHPreprocessingEdge, VectorEdgeRange<CHPreprocessingEdge>>, CHPreprocessingEdge>>;
void Print(const std::vector<CHPreprocessingEdge>& edges, const std::string& name);

void Print(const std::vector<CHPreprocessingEdge>& edges, const std::string& name) {
    std::cout << name << " :" << std::endl;
    for(auto&& edge : edges) {
        edge.Print();
    }
}


struct FilterShortcutsParameter {
    std::vector<CHPreprocessingEdge> input_shortcuts;
    std::vector<CHPreprocessingEdge> expected_shortcuts;

    FilterShortcutsParameter(const std::vector<CHPreprocessingEdge>& is, const std::vector<CHPreprocessingEdge>& es) :
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
            std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 1, 4, 1}},
            std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 1, 4, 1}}
        },
        FilterShortcutsParameter{ 
            std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 1, 3, 1}, CHPreprocessingEdge{11, 1, 4, 1}},
            std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 1, 3, 1}, CHPreprocessingEdge{11, 1, 4, 1}}
        },
        FilterShortcutsParameter{ 
            std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 1, 6, 1}, CHPreprocessingEdge{11, 1, 4, 1}, CHPreprocessingEdge{12, 1, 6, 2}},
            std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 1, 6, 1}, CHPreprocessingEdge{11, 1, 4, 1}}
        },
        FilterShortcutsParameter{ 
            std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 1, 3, 1}, CHPreprocessingEdge{11, 1, 4, 1}, CHPreprocessingEdge{12, 1, 3, 1}},
            std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 1, 3, 1}, CHPreprocessingEdge{11, 1, 4, 1}}
        },
        FilterShortcutsParameter{ 
            std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 1, 6, 4}, CHPreprocessingEdge{11, 1, 3, 2}, CHPreprocessingEdge{12, 1, 6, 3}},
            std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{11, 1, 3, 2}, CHPreprocessingEdge{12, 1, 6, 3}}
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
            std::vector<CHPreprocessingEdge>{
                CHPreprocessingEdge{10, 1, 4, 1, CHPreprocessingEdge::EdgeType::forward},
                CHPreprocessingEdge{10, 4, 1, 1, CHPreprocessingEdge::EdgeType::forward}
            },
            std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 1, 4, 1, CHPreprocessingEdge::EdgeType::twoway}}
        },
        FilterShortcutsParameter{ 
            std::vector<CHPreprocessingEdge>{
                CHPreprocessingEdge{10, 1, 4, 3, CHPreprocessingEdge::EdgeType::forward},
                CHPreprocessingEdge{10, 4, 1, 1, CHPreprocessingEdge::EdgeType::forward}
            },
            std::vector<CHPreprocessingEdge>{
                CHPreprocessingEdge{10, 1, 4, 3, CHPreprocessingEdge::EdgeType::forward},
                CHPreprocessingEdge{10, 4, 1, 1, CHPreprocessingEdge::EdgeType::forward}
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

