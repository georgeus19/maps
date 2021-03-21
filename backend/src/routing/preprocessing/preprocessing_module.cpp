#include "routing/graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/edges/contraction_edge.h"
#include "routing/algorithm.h"
#include "routing/dijkstra.h"
#include "routing/exception.h"
#include "database/database_helper.h"
#include "routing/graph.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/vertices/contraction_vertex.h"
#include "utility/point.h"
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

using G = Graph<ContractionVertex<ContractionEdge>, ContractionEdge>;
const string kDbName = "gis";
const string kUser = "postgres";
const string kPassword = "wtz2trln";
const string kHostAddress = "127.0.0.1";
const string kPort = "5432";
int main(int argv, const char ** argc) {
    DatabaseHelper d{kDbName, kUser, kPassword, kHostAddress, kPort};
    G g{};
    std::cout << "Load graph from czedges." << std::endl;
    d.LoadFullGraph<G>("czedges", g);

    std::cout << "Contract graph from czedges." << std::endl;
    ContractionParameters parameters{20000000};
    GraphContractor<G> c{g, parameters};
    c.ContractGraph();
    std::cout << "Contration done." << std::endl;

    // Save shortcuts.
    // Save vertex ordering.

}