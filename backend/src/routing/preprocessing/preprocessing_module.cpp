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
#include "pqxx/except.hxx"

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
int main(int argc, const char ** argv) {
    if (argc != 2) {
        std::cout << "There must be 1 argument: input_graph_table" << std::endl;
        return 1;
    }

    try {
        DatabaseHelper d{kDbName, kUser, kPassword, kHostAddress, kPort};
        G g{};
        std::string input_graph_table{argv[1]};
        std::string output_graph_table{"CH" + input_graph_table};
        std::string output_ordering_table{output_graph_table + "_vertex_ordering"};
      
        std::cout << "Load graph from " << input_graph_table << "." << std::endl;
        UnpreprocessedDbGraph unpreprocessed_db_graph{};
        d.LoadFullGraph<G>(input_graph_table, g, &unpreprocessed_db_graph);

        std::cout << "Vertices: " << g.GetVertexCount() << std::endl;

        std::cout << "Edges before contraction: " << g.GetEdgeCount() << std::endl;

        ContractionParameters parameters{d.GetMaxEdgeId(input_graph_table), 5, 190, 120, 0};
        GraphContractor<G> c{g, parameters};
        c.ContractGraph();
        std::cout << "Contraction done." << std::endl;

        std::cout << "Edges after contraction: " << g.GetEdgeCount() << std::endl;

        CHDbGraph ch_db_graph{};
        d.CreateGraphTable(input_graph_table, output_graph_table, &ch_db_graph);

        d.DropGeographyIndex(output_graph_table);
        std::cout << "Geography index dropped." << std::endl;
        
        // Save shortcuts.
        d.AddShortcuts(output_graph_table, g);
        std::cout << "Shortcuts added to " + output_graph_table << "."<< std::endl;
        d.CreateGeographyIndex(output_graph_table);
        std::cout << "Geography index restored." << std::endl;

        d.AddVertexOrdering(output_ordering_table, g);
        std::cout << "Vertex ordering saved to " << output_ordering_table << "." << std::endl;


    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    
}