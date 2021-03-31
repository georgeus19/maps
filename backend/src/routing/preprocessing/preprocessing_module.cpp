#include "routing/graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/edges/ch_preprocessing_edge.h"
#include "routing/algorithm.h"
#include "routing/dijkstra.h"
#include "routing/exception.h"
#include "database/database_helper.h"
#include "routing/graph.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/vertices/contraction_vertex.h"
#include "utility/point.h"
#include "routing/bidirectional_graph.h"
#include "routing/preprocessing/vertex_measures.h"
#include "routing/preprocessing/graph_contractor.h"


#include <vector>
#include <tuple>
#include <string>
#include <iostream>

using namespace std;
using namespace database;
using namespace routing;
using namespace preprocessing;

using G = BidirectionalGraph<ContractionVertex<CHPreprocessingEdge>, CHPreprocessingEdge>;
const string kDbName = "gis";
const string kUser = "postgres";
const string kPassword = "wtz2trln";
const string kHostAddress = "127.0.0.1";
const string kPort = "5432";
int main(int argv, const char ** argc) {
    try {
        DatabaseHelper d{kDbName, kUser, kPassword, kHostAddress, kPort};
        G g{};
        std::string table_name{"cznoloops"};
        // std::string table_name{"lux31"};
      
        std::cout << "Load graph from cznoloops." << std::endl;
        UnpreprocessedDbGraph db_graph{};
        d.LoadFullGraph<G>(table_name, g, &db_graph);
        size_t count = 0;
        g.ForEachVertex([&](typename G::V&){
            ++count;
        });
        std::cout << "vertices: " << count << std::endl;

        size_t ecount = 0;
        g.ForEachEdge([&](typename G::E&){
            ++ecount;
        });
        std::cout << "edges before contraction: " << ecount << std::endl;

        std::cout << "Contract graph from czedges." << std::endl;
        ContractionParameters parameters{d.GetMaxEdgeId(table_name), 5, 190, 120, 1};
        GraphContractor<G> c{g, parameters};
        c.ContractGraph();
        std::cout << "Contraction done." << std::endl;

        size_t eaccount = 0;
        g.ForEachEdge([&](typename G::E&){
            ++eaccount;
        });
        std::cout << "edges after contraction: " << eaccount << std::endl;

        // bool columns_added = d.AddShortcutColumns(table_name);
        // std::cout << "Add shortcut columns if not there -> " << (columns_added ? "added" : "were already present") << "." << std::endl;
        
        // // Save shortcuts.
        // d.AddShortcuts(table_name, g);
        // // Save vertex ordering.
        // d.AddVertexOrdering("vertex_ordering_table", g);

    } catch (const std::exception& e) {
        e.what();
    }
    
}