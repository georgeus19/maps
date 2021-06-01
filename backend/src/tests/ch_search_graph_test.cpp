#include "gtest/gtest.h"
#include "gmock/gmock.h"  
#include "routing/adjacency_list_graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/algorithm.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/query/dijkstra.h"
#include "routing/bidirectional_graph.h"
#include "routing/query/bidirectional_dijkstra.h"
#include "routing/preprocessing/graph_contractor.h"
#include "routing/exception.h"
#include "database/database_helper.h"
#include "utility/point.h"
#include "tests/graph_test.h"
#include "routing/edges/ch_edge.h"
#include "routing/ch_search_graph.h"
#include "routing/vertices/ch_vertex.h"
#include "routing/edge_ranges/vector_edge_range.h"
#include "routing/edge_ranges/iterator_edge_range.h"
#include "routing/edges/length_source.h"

#include <string>
#include <vector>

using namespace std;
using namespace routing;
using namespace database;
using namespace query;
using namespace preprocessing;
using Edge = CHEdge<NumberLengthSource>;
using G = BidirectionalGraph<AdjacencyListGraph<CHVertex<Edge, VectorEdgeRange<Edge>>, Edge>>;
using SearchGraph = CHSearchGraph<CHVertex<Edge, IteratorEdgeRange<Edge, std::vector<Edge>::iterator>>, Edge>;

TEST(CHSearchGraphTests, ShortcutsAndBackwardEdges) {
    G g{};
    TestBidirectedSearchGraph(g);
    SearchGraph search_graph{};
    search_graph.Load(g);
    std::vector<Edge> expected{
        typename SearchGraph::Edge{0, 1, 2, 2, SearchGraph::Edge::EdgeType::forward},
        typename SearchGraph::Edge{1, 3, 1, 3, SearchGraph::Edge::EdgeType::twoway},
        typename SearchGraph::Edge{2, 2, 6, 12, SearchGraph::Edge::EdgeType::backward},
        typename SearchGraph::Edge{3, 3, 4, 2, SearchGraph::Edge::EdgeType::twoway},
        typename SearchGraph::Edge{4, 4, 5, 3, SearchGraph::Edge::EdgeType::twoway},
        typename SearchGraph::Edge{5, 4, 6, 6, SearchGraph::Edge::EdgeType::forward},
        typename SearchGraph::Edge{6, 5, 6, 2, SearchGraph::Edge::EdgeType::backward},
        typename SearchGraph::Edge{7, 6, 5, 3, SearchGraph::Edge::EdgeType::forward},
        typename SearchGraph::Edge{8, 4, 1, 3, SearchGraph::Edge::EdgeType::twoway},
        typename SearchGraph::Edge{9, 1, 5, 8, SearchGraph::Edge::EdgeType::twoway},
        typename SearchGraph::Edge{10, 1, 6, 11, SearchGraph::Edge::EdgeType::forward},
        typename SearchGraph::Edge{11, 5, 2, 10, SearchGraph::Edge::EdgeType::backward},
        typename SearchGraph::Edge{11, 2, 5, 15, SearchGraph::Edge::EdgeType::forward}
    };
    std::vector<Edge> actual{};
    search_graph.ForEachEdge([&](const typename SearchGraph::Edge& edge) {
        edge.Print();
        actual.push_back(edge);
    });

    EXPECT_THAT(actual, testing::UnorderedElementsAreArray(expected));
}