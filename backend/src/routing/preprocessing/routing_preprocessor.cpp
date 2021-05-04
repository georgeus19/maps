#include "routing/edges/basic_edge.h"
#include "routing/edges/ch_preprocessing_edge.h"
#include "routing/algorithm.h"
#include "routing/query/dijkstra.h"
#include "routing/exception.h"
#include "database/database_helper.h"
#include "routing/adjacency_list_graph.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/vertices/contraction_vertex.h"
#include "utility/point.h"
#include "routing/bidirectional_graph.h"
#include "routing/preprocessing/vertex_measures.h"
#include "routing/preprocessing/graph_contractor.h"
#include "routing/vertices/ch_vertex.h"
#include "routing/edge_ranges/vector_edge_range.h"
#include "routing/preprocessing/ch_data_manager.h"

#include <chrono>
#include <vector>
#include <tuple>
#include <string>
#include <iostream>
#include "pqxx/except.hxx"

using namespace std;
using namespace database;
using namespace routing;
using namespace preprocessing;

static void RunCHPreprocessing(const std::string& base_graph_table);
const string kDbName = "gis";
const string kUser = "postgres";
const string kPassword = "wtz2trln";
const string kHostAddress = "127.0.0.1";
const string kPort = "5432";
int main(int argc, const char ** argv) {
    if (argc != 2) {
        std::cout << "There must be 1 argument: base_graph_table" << std::endl;
        return 1;
    }

    try {
        RunCHPreprocessing(argv[1]);

    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    
}

static void RunCHPreprocessing(const std::string& base_graph_table) {
    DatabaseHelper d{kDbName, kUser, kPassword, kHostAddress, kPort};
    std::string output_graph_table{"CH" + base_graph_table};
    std::string output_ordering_table{output_graph_table + "_vertex_ordering"};
    
    std::cout << "Load graph from " << base_graph_table << "." << std::endl;
    CHDataManager manager{d};
    CHDataManager::Graph g = manager.LoadBaseGraph(base_graph_table);

    std::cout << "Vertices: " << g.GetVertexCount() << std::endl;

    std::cout << "Edges before contraction: " << g.GetEdgeCount() << std::endl;

    ContractionParameters parameters{d.GetMaxEdgeId(base_graph_table), 5, 190, 120, 0};
    GraphContractor<CHDataManager::Graph> c{g, parameters};
    c.ContractGraph();
    std::cout << "Contraction done." << std::endl;

    std::cout << "Edges after contraction: " << g.GetEdgeCount() << std::endl;
    manager.SaveNewGraph(g, base_graph_table, output_graph_table);
}