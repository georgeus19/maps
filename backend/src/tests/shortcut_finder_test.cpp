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

#include <string>
#include <vector>
#include <tuple>
#include <string_view>

using namespace std;
using namespace routing;
using namespace database;
using namespace preprocessing;

using G = BidirectionalGraph<ContractionVertex<CHPreprocessingEdge>, CHPreprocessingEdge>;

TEST(ShortcutFinderGeographyTest, SimpleGeographyCopyTest) {
    G g{};
    std::string expected_geography{"Geo121"};
    g.AddEdge(std::move(CHPreprocessingEdge{0, 1, 2, 1, 0, expected_geography}));

    g.AddEdge(std::move(CHPreprocessingEdge{1, 2, 3, 1, 0, "Geo231"}));

    ShortcutFinder<G> shortcut_finder{g, ContractionParameters{11, 5, 1, 1, 0}};
    auto&& shortcuts = shortcut_finder.FindShortcuts(g.GetVertex(2));
    
    std::cout << shortcuts[0].get_geography() << std::endl;
    EXPECT_EQ(shortcuts.size(), 1);
    EXPECT_EQ(shortcuts[0].get_geography(), expected_geography);
    
}