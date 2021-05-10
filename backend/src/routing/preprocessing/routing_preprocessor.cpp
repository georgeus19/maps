#include "routing/edges/basic_edge.h"
#include "routing/edges/ch_preprocessing_edge.h"

#include "routing/edge_ranges/vector_edge_range.h"

#include "routing/algorithm.h"
#include "routing/query/dijkstra.h"
#include "routing/exception.h"
#include "routing/configuration_parser.h"
#include "routing/adjacency_list_graph.h"
#include "routing/bidirectional_graph.h"

#include "routing/vertices/basic_vertex.h"
#include "routing/vertices/contraction_vertex.h"
#include "routing/vertices/ch_vertex.h"

#include "routing/preprocessing/vertex_measures.h"
#include "routing/preprocessing/graph_contractor.h"
#include "routing/preprocessing/ch_data_manager.h"

#include "routing/profile/profile_generator.h"
#include "routing/profile/profile.h"

#include "utility/point.h"

#include "database/database_helper.h"

#include <chrono>
#include <vector>
#include <tuple>
#include <string>
#include <unordered_map>
#include <iostream>
#include "pqxx/except.hxx"

using namespace std;
using namespace database;
using namespace routing;
using namespace preprocessing;
using namespace profile;

static void RunCHPreprocessing(const Configuration& cfg);
int main(int argc, const char ** argv) {
    if (argc != 2) {
        std::cout << "There must be 1 argument: config_path" << std::endl;
        return 1;
    }

    try {
        std::unordered_map<std::string, std::function<void(const Configuration&)>> algorithms{
            {"ch", RunCHPreprocessing}
        };
        std::string config_path = argv[1];
        ConfigurationParser parser{config_path};
        auto&& cfg = parser.Parse();
        algorithms[cfg.algorithm->name](cfg);

    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}

template <typename Graph>
static void SetProfile(Graph& graph, const std::vector<ProfileProperty>& profile_properties) {

}

static void CreateGraphs(const Configuration& cfg) {
    for(auto&& profile_property : cfg.profile_properties) {

    }
}

static void RunCHPreprocessing(const Configuration& cfg) {
    DatabaseHelper d{cfg.database.name, cfg.database.user, cfg.database.password, cfg.database.host, cfg.database.port};
    std::string output_graph_table{"CH" + cfg.algorithm->base_graph_table};
    std::string output_ordering_table{output_graph_table + "_vertex_ordering"};
    
    std::cout << "Load graph from " << cfg.algorithm->base_graph_table << "." << std::endl;
    CHDataManager manager{d};
    CHDataManager::Graph g = manager.LoadBaseGraph(cfg.algorithm->base_graph_table);

    std::unordered_map<std::string, std::function<void(ProfileGenerator&, ProfileProperty&)>> indicies{
        {"green", [](ProfileGenerator& gen, ProfileProperty& prop){ gen.AddGreenIndex(prop.table_name, std::move(prop.options)); }}
    };
    ProfileGenerator gen{d, cfg.algorithm->base_graph_table, g.GetMaxEdgeId(), 100};

    
    
    std::cout << "Vertices: " << g.GetVertexCount() << std::endl;

    std::cout << "Edges before contraction: " << g.GetEdgeCount() << std::endl;

    ContractionParameters parameters{d.GetMaxEdgeId(cfg.algorithm->base_graph_table), 5, 190, 120, 0};
    GraphContractor<CHDataManager::Graph> c{g, parameters};
    c.ContractGraph();
    std::cout << "Contraction done." << std::endl;

    std::cout << "Edges after contraction: " << g.GetEdgeCount() << std::endl;
    manager.SaveNewGraph(g, cfg.algorithm->base_graph_table, output_graph_table);
}