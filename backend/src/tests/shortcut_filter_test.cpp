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
#include "routing/bidirectional_graph.h"
#include "routing/preprocessing/contraction_parameters.h"
#include "routing/preprocessing/bidirectional_dijkstra.h"
#include "routing/preprocessing/shortcut_finder.h"
#include "routing/preprocessing/shortcut_filter.h"
#include "routing/preprocessing/shortcut_container.h"

#include <string>
#include <vector>
#include <tuple>
#include <string_view>

using namespace std;
using namespace routing;
using namespace database;
using namespace preprocessing;

using G = BidirectionalGraph<ContractionVertex<CHPreprocessingEdge>, CHPreprocessingEdge>;
void Print(const std::vector<CHPreprocessingEdge>& edges, const std::string& name);

struct ClassifyShortcutsParameter {
    std::vector<CHPreprocessingEdge> input_shortcuts;
    ShortcutContainer<CHPreprocessingEdge> expected_shortcuts;

    ClassifyShortcutsParameter(const std::vector<CHPreprocessingEdge>& is, const ShortcutContainer<CHPreprocessingEdge>& es) :
        input_shortcuts(is), expected_shortcuts(es) {}
};

class ShortcutFilterClassifyShortcutsTests : public testing::TestWithParam<ClassifyShortcutsParameter> {
    protected:
    
    G g_;
    void SetUp() override {
        TestBasicReverseGraph(g_);
    }
};

INSTANTIATE_TEST_CASE_P(
    ShortcutFilterClassifyShortcutsTestParameters, 
    ShortcutFilterClassifyShortcutsTests,
    ::testing::Values(
        ClassifyShortcutsParameter{ // One edge goes to improving
            std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 1, 3, 1}},
            ShortcutContainer<CHPreprocessingEdge>{
                std::vector<CHPreprocessingEdge>{},
                std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 1, 3, 1}}
            }
        },
        ClassifyShortcutsParameter{ // More edges go to improving, one discarded.
            std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 1, 3, 1}, CHPreprocessingEdge{11, 4, 6, 5}, CHPreprocessingEdge{12, 2, 6, 9}},
            ShortcutContainer<CHPreprocessingEdge>{
                std::vector<CHPreprocessingEdge>{},
                std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 1, 3, 1}, CHPreprocessingEdge{11, 4, 6, 5}}
            }
        },
        ClassifyShortcutsParameter{ // One edge goes to new edges.
            std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 1, 6, 10}},
            ShortcutContainer<CHPreprocessingEdge>{
                std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 1, 6, 10}},
                std::vector<CHPreprocessingEdge>{}
            }
        },
        ClassifyShortcutsParameter{ // More edges go to new edges, one discarded.
            std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 1, 6, 10}, CHPreprocessingEdge{11, 1, 5, 10}, CHPreprocessingEdge{12, 1, 2, 10}},
            ShortcutContainer<CHPreprocessingEdge>{
                std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 1, 6, 10}, CHPreprocessingEdge{11, 1, 5, 10}},
                std::vector<CHPreprocessingEdge>{}
            }
        },
        ClassifyShortcutsParameter{ // new edges, improve edges, discarded edges. - I, N, D, N, N, D, I, I, D
            std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 4, 5, 1}, CHPreprocessingEdge{11, 5, 1, 10}, CHPreprocessingEdge{12, 1, 2, 3},
                CHPreprocessingEdge{13, 1, 4, 10}, CHPreprocessingEdge{14, 1, 6, 10}, CHPreprocessingEdge{15, 3, 4, 42}, CHPreprocessingEdge{16, 4, 3, 1},
                CHPreprocessingEdge{17, 4, 6, 2}, CHPreprocessingEdge{18, 5, 3, 12} 
            },
            ShortcutContainer<CHPreprocessingEdge>{
                std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{11, 5, 1, 10}, CHPreprocessingEdge{13, 1, 4, 10}, CHPreprocessingEdge{14, 1, 6, 10}},
                std::vector<CHPreprocessingEdge>{CHPreprocessingEdge{10, 4, 5, 1}, CHPreprocessingEdge{16, 4, 3, 1}, CHPreprocessingEdge{17, 4, 6, 2}}
            }
        }
    )
);

TEST_P(ShortcutFilterClassifyShortcutsTests, ClassifyShortcutsSimpleTest) {
    ClassifyShortcutsParameter param = GetParam();
    auto input = param.input_shortcuts;
    ShortcutFilter<G> filter{g_};
    auto&& actual = filter.ClassifyShortcuts(std::move(param.input_shortcuts));
    Print(input, "Input shortcuts");
    Print(actual.new_edges, "Actual new edges");
    Print(param.expected_shortcuts.new_edges, "Expected new edges");

    Print(actual.improving_edges, "Actual improve edges");
    Print(param.expected_shortcuts.improving_edges, "Expected improve edges");
    EXPECT_THAT(actual.new_edges, testing::ElementsAreArray(param.expected_shortcuts.new_edges));
    EXPECT_THAT(actual.improving_edges, testing::ElementsAreArray(param.expected_shortcuts.improving_edges));
}

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


