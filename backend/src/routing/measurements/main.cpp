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
#include "routing/ch_search_graph.h"
#include <string>
#include <vector>
#include <tuple>
#include <chrono>
#include "routing/vertices/ch_search_vertex.h"
#include "routing/vertices/edge_range_vertex.h"

using namespace std;
using namespace routing;
using namespace query;
using namespace database;
using namespace preprocessing;

using G = BidirectionalGraph<AdjacencyListGraph<ContractionSearchVertex<CHSearchEdge>, CHSearchEdge>>;
using GImmutable = CHSearchGraph<CHSearchVertex<CHSearchEdge, std::vector<CHSearchEdge>::iterator>, CHSearchEdge>;
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

void LoadGraph(G& g) {
    database::DatabaseHelper d{kDbName, kUser, kPassword, kHostAddress, kPort};
    CHDbGraph db_graph{};
    std::cout << "Database helper init (ctor): " << std::endl;
    PrintMemoryUsage();
    // d.LoadFullGraph("CHcznoloops", g, &db_graph);
    d.LoadFullGraph("abc", g, &db_graph);
    d.LoadAdditionalVertexProperties("abc_vertex_ordering", g);
    
    std::cout << "vertex size: " << sizeof(ContractionSearchVertex<CHSearchEdge>) << std::endl;
    std::cout << "vertex count: " << g.GetVertexCount() << std::endl;
    std::cout << "edge size: " << sizeof(CHSearchEdge) << std::endl;
    std::cout << "edge count: " << g.GetEdgeCount() << std::endl; 
    PrintMemoryUsage();

}


void LoadImmutableGraph(GImmutable& gi) {
    G g{};
    LoadGraph(g);
    gi.Load(g);
    PrintMemoryUsage();
    G gg{};
    LoadGraph(gg);
    std::cout << "Normal and immutable graph in memory: " << std::endl;
    PrintMemoryUsage();
}

int main(int argc, const char ** argv) {
    std::cout <<  "std::vector<CHSearchEdge>::iterator " << sizeof(std::vector<CHSearchEdge>::iterator) << std::endl;
    std::cout << "CHSearchVertex<CHSearchEdge, std::vector<CHSearchEdge>::iterator> " << sizeof(CHSearchVertex<CHSearchEdge, std::vector<CHSearchEdge>::iterator>) << std::endl;
    std::cout << "ContractionSearchVertex<CHSearchEdge> " << sizeof(ContractionSearchVertex<CHSearchEdge>) << std::endl;
    std::cout << "Program starting: " << std::endl;
    PrintMemoryUsage();
    // G g{};
    GImmutable g{};
    PrintMemoryUsage();
    // LoadGraph(g);
    LoadImmutableGraph(g);
    std::cout << "vertex size: " << sizeof(ContractionSearchVertex<CHSearchEdge>) << std::endl;
    std::cout << "vertex count: " << g.GetVertexCount() << std::endl;
    std::cout << "edge size: " << sizeof(CHSearchEdge) << std::endl;
    std::cout << "edge count: " << g.GetEdgeCount() << std::endl; 
    std::cout << "Init graph (ctor): " << std::endl;
    PrintMemoryUsage();
}