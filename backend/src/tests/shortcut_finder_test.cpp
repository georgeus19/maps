#include "gtest/gtest.h"
#include "gmock/gmock.h"  
#include "routing/adjacency_list_graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/edges/ch_preprocessing_edge.h"
#include "routing/algorithm.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/vertices/contraction_vertex.h"
#include "routing/query/dijkstra.h"
#include "routing/exception.h"
#include "database/database_helper.h"
#include "utility/point.h"
#include "routing/endpoint_handler.h"
#include "tests/graph_test.h"
#include "routing/preprocessing/vertex_measures.h"
#include "routing/preprocessing/graph_contractor.h"
#include "routing/bidirectional_graph.h"
#include "routing/preprocessing/contraction_parameters.h"
#include "routing/query/bidirectional_dijkstra.h"
#include "routing/preprocessing/shortcut_finder.h"
#include "routing/vertices/ch_vertex.h"
#include "routing/edge_ranges/vector_edge_range.h"

#include <string>
#include <vector>
#include <tuple>
#include <string_view>

using namespace std;
using namespace routing;
using namespace database;
using namespace query;
using namespace preprocessing;

// using G = BidirectionalGraph<AdjacencyListGraph<ContractionVertex<CHPreprocessingEdge>, CHPreprocessingEdge>>;
using G = BidirectionalGraph<AdjacencyListGraph<CHVertex<CHPreprocessingEdge, VectorEdgeRange<CHPreprocessingEdge>>, CHPreprocessingEdge>>;