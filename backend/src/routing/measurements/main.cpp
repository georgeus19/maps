#include "routing/adjacency_list_graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/edges/ch_preprocessing_edge.h"
#include "routing/algorithm.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/query/dijkstra.h"
#include "routing/exception.h"
#include "database/database_helper.h"
#include "utility/point.h"
#include "routing/bidirectional_graph.h"
#include "tests/graph_test.h"
#include "routing/preprocessing/vertex_measures.h"
#include "routing/preprocessing/graph_contractor.h"
#include "routing/query/setup.h"
#include "utility/point.h"
#include "routing/ch_search_graph.h"
#include <string>
#include <vector>
#include <tuple>
#include <chrono>
#include "routing/edge_ranges/iterator_edge_range.h"
#include "routing/edge_ranges/vector_edge_range.h"
#include "routing/vertices/ch_vertex.h"
#include "routing/configuration_parser.h"

using namespace std;
using namespace routing;
using namespace query;
using namespace database;
using namespace preprocessing;

using G = BidirectionalGraph<AdjacencyListGraph<CHVertex<CHSearchEdge, VectorEdgeRange<CHSearchEdge>>, CHSearchEdge>>;
using SearchGraph = CHSearchGraph<CHVertex<CHSearchEdge, IteratorEdgeRange<CHSearchEdge, std::vector<CHSearchEdge>::iterator>>, CHSearchEdge>;
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "sys/types.h"
#include "sys/sysinfo.h"

void process_mem_usage(double& vm_usage, double& resident_set)
{
    vm_usage     = 0.0;
    resident_set = 0.0;

    // the two fields we want
    unsigned long vsize;
    long rss;
    {
        std::string ignore;
        std::ifstream ifs("/proc/self/stat", std::ios_base::in);
        ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
                >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
                >> ignore >> ignore >> vsize >> rss;
    }

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    vm_usage = vsize / 1024.0 / 1024.0; // in MB
    resident_set = rss * page_size_kb;
}

void PrintMemoryUsage() {
    using std::cout;
    using std::endl;

    double vm, rss;
    process_mem_usage(vm, rss);
    cout << "VM: " << vm << "; RSS: " << rss << endl;
}

void LoadGraph(DatabaseHelper& d, G& g) {
    
    CHDbGraph db_graph{};
    std::cout << "Database helper init (ctor): " << std::endl;
    PrintMemoryUsage();
    // d.LoadFullGraph("CHcznoloops", g, &db_graph);
    d.LoadFullGraph("CHczedges", g, &db_graph);
    d.LoadAdditionalVertexProperties("CHczedges_vertex_ordering", g);
    
    std::cout << "vertex size: " << sizeof(CHVertex<CHSearchEdge, IteratorEdgeRange<CHSearchEdge, std::vector<CHSearchEdge>::iterator>>) << std::endl;
    std::cout << "vertex count: " << g.GetVertexCount() << std::endl;
    std::cout << "edge size: " << sizeof(CHSearchEdge) << std::endl;
    std::cout << "edge count: " << g.GetEdgeCount() << std::endl; 
    PrintMemoryUsage();

}


void LoadImmutableGraph(DatabaseHelper& d, SearchGraph& gi) {
    G g{};
    LoadGraph(d, g);
    gi.Load(g);
    std::cout << "Search graph load: " << std::endl;
    PrintMemoryUsage();
    G gg{};
    LoadGraph(d, gg);
    std::cout << "Normal and immutable graph in memory: " << std::endl;
    PrintMemoryUsage();
}

int main(int argc, const char ** argv) {
    ConfigurationParser parser{argv[1]};
    auto&& cfg = parser.Parse();
    DatabaseHelper d{cfg.database.name, cfg.database.user, cfg.database.password, cfg.database.host, cfg.database.port};
    std::cout <<  "std::vector<CHSearchEdge>::iterator " << sizeof(std::vector<CHSearchEdge>::iterator) << std::endl;
    std::cout << "Program starting: " << std::endl;
    PrintMemoryUsage();
    // G g{};
    SearchGraph g{};
    PrintMemoryUsage();
    // LoadGraph(d, g);
    LoadImmutableGraph(d, g);
    std::cout << "vertex count: " << g.GetVertexCount() << std::endl;
    std::cout << "edge size: " << sizeof(CHSearchEdge) << std::endl;
    std::cout << "edge count: " << g.GetEdgeCount() << std::endl; 
    std::cout << "Init graph (ctor): " << std::endl;
    PrintMemoryUsage();
}