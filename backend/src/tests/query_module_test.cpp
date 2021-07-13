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
#include "routing/utility/point.h"
#include "routing/query/router.h"
#include "routing/edges/length_source.h"

#include <string>
#include <vector>
#include <tuple>
#include <chrono>

using namespace std;
using namespace routing;
using namespace query;
using namespace database;
using namespace preprocessing;


#include <iostream>
#include <fstream>
#include <unistd.h>


TEST(QueryModuleTests, QueryModuleTest) {
    // std::string table_name = "czedges";
    // utility::Point source{13.393973958925121, 49.73380001564838};
    // utility::Point target{13.399605229319672,  49.72901499324271};
    // // // lon lat
    //  std::string edge_table = "CHczedges";
    // auto&& g = CHFactory::CreateGraph(edge_table);
    // Router<CHFactory> router{g, edge_table};
    // auto&& result = router.CalculateShortestRoute("CHczedges", source, target);
    // utility::Point source{13.376990, 49.746841}; // plzen
    // // utility::Point target{15.608720, 50.627522}; // vrchlabi
    // utility::Point target{13.391600, 49.934470}; // plasy
    // std::cout << "Dijkstra query:" << std::endl;
    // auto start = std::chrono::high_resolution_clock::now();
    // auto&& result = CCalculateShortestRoute<DijkstraFactory>(table_name, source, target);
    // auto finish = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> elapsed = finish - start;
    // std::cout << "Elapsed time: " << elapsed.count() << " s\n";
    // std::cout << "Contraction hierarchies query:" << std::endl;
    // start = std::chrono::high_resolution_clock::now();
    // auto&& CHresult = CCalculateShortestRoute<CHFactory>("CH" + table_name, source, target);
    // finish = std::chrono::high_resolution_clock::now();
    // elapsed = finish - start;
    // std::cout << "Elapsed time: " << elapsed.count() << " s\n";
    // // std::cout << result << std::endl;
    // // std::cout << CHresult << std::endl;
    // EXPECT_EQ(result, CHresult);
    // ;
}


