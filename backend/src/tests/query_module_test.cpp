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
#include <chrono>

using namespace std;
using namespace routing;
using namespace query;
using namespace database;
using namespace preprocessing;


TEST(QueryModuleTests, QueryModuleTest) {
    std::string table_name = "cznoloops";
    utility::Point source{15.568861848533453, 50.66747073900733};
    utility::Point target{15.581821585773252,  50.67989977147217};
    // lon lat
    // utility::Point source{13.376990, 49.746841}; // plzen
    // utility::Point target{15.608720, 50.627522}; // vrchlabi
    // utility::Point target{13.391600, 49.934470}; // plasy
    std::cout << "Dijkstra query:" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    auto&& result = CCalculateShortestRoute<DijkstraSetup>(table_name, source, target);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";
    std::cout << "Contraction hierarchies query:" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    auto&& CHresult = CCalculateShortestRoute<CHSetup>("CH" + table_name, source, target);
    finish = std::chrono::high_resolution_clock::now();
    elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";
    // std::cout << result << std::endl;
    // std::cout << CHresult << std::endl;
    EXPECT_EQ(result, CHresult);
    ;
}


