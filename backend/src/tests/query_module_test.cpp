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
#include "routing/basic_edge_endpoint_handler.h"
#include "routing/bidirectional_graph.h"
#include "tests/graph_test.h"
#include "routing/preprocessing/vertex_measures.h"
#include "routing/preprocessing/graph_contractor.h"
#include "routing/query/module.h"
#include "utility/point.h"
#include <string>
#include <vector>
#include <tuple>

using namespace std;
using namespace routing;
using namespace query;
using namespace database;
using namespace preprocessing;


TEST(QueryModuleTests, QueryModuleTest) {
    std::string table_name = "cznoloops";
    utility::Point start{15.568861848533453, 50.66747073900733};
    utility::Point end{15.581821585773252,  50.67989977147217};


    auto&& result = CCalculateShortestRoute<DijkstraSetup>(table_name, start, end);
    auto&& CHresult = CCalculateShortestRoute<CHSetup>("CH" + table_name, start, end);
    std::cout << result << std::endl;
    std::cout << CHresult << std::endl;
    EXPECT_EQ(result, CHresult);
    ;
}


