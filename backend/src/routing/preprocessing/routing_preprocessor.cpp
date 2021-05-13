#include "routing/edges/basic_edge.h"
#include "routing/edges/ch_preprocessing_edge.h"

#include "routing/edge_ranges/vector_edge_range.h"

#include "routing/table_name_repository.h"
#include "routing/algorithm.h"
#include "routing/query/dijkstra.h"
#include "routing/exception.h"
#include "routing/configuration_parser.h"
#include "routing/adjacency_list_graph.h"
#include "routing/bidirectional_graph.h"
#include "routing/constants.h"

#include "routing/vertices/basic_vertex.h"
#include "routing/vertices/contraction_vertex.h"
#include "routing/vertices/ch_vertex.h"

#include "routing/preprocessing/vertex_measures.h"
#include "routing/preprocessing/graph_contractor.h"

#include "routing/profile/profile_generator.h"
#include "routing/profile/profile.h"

#include "utility/point.h"

#include "database/database_helper.h"

#include "toml11/toml.hpp"

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

static void CreateIndicies(const std::string& path);
static void RunCHPreprocessing(Configuration&& cfg);
static void PrintHelp();

int main(int argc, const char ** argv) {
    if (argc != 3) {
        PrintHelp();
        return 1;
    }

    try {
        if (std::string{argv[1]} == "--create-index") {
            CreateIndicies(argv[2]);
            return 0;
        }

        if (std::string{argv[1]} == "--algorithm") {
            std::unordered_map<std::string, std::function<void(Configuration&&)>> algorithms{
                {"ch", RunCHPreprocessing}
            };
            std::string config_path = argv[2];
            ConfigurationParser parser{config_path};
            auto&& cfg = parser.Parse();
            algorithms[cfg.algorithm->name](std::move(cfg));
            return 0;
        }
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    PrintHelp();
    return 1;

}

static void CreateIndicies(const std::string& path) {
    auto&& data = toml::parse(path);
    DatabaseConfig db_cfg{toml::find(data, Constants::Input::TableNames::kDatabase)};
    DatabaseHelper d{db_cfg.name, db_cfg.user, db_cfg.password, db_cfg.host, db_cfg.port};
    std::unordered_map<std::string, std::function<void(const toml::value&)>> indicies{
        {
            Constants::IndexNames::kGreenIndex,
            [&](const toml::value& table){
                GreenIndex index{};
                index.Create(d, 
                    toml::find<std::string>(table, Constants::Input::Indicies::kEdgesTable),
                    toml::find<std::string>(table, Constants::Input::Indicies::kPolygonTable),
                    toml::find<std::string>(table, Constants::Input::Indicies::kIndexTable)
                ); 
            }
        },
        {Constants::IndexNames::kLengthIndex, [](const toml::value&){ /* no action necessary... - already present in base graph! */ }}
    };
    for(auto&& index : toml::find<toml::array>(data, Constants::Input::TableNames::kIndicies)) {
        std::string name = toml::find<std::string>(index, Constants::Input::kName);
        if (!indicies.contains(name)) {
            std::cout << "No index " << name << " exists." << std::endl;
            continue;
        }
        indicies[name](index);
        std::cout << "Index " << name << " created." << std::endl;
    }
}

static std::vector<profile::Profile> GenerateProfiles(DatabaseHelper& d, Configuration& cfg, double scale_max) {
    ProfileGenerator gen{d, cfg.algorithm->base_graph_table, scale_max};
    for(auto&& prop : cfg.profile_properties) {
        prop.index->Load(d, prop.table_name);
        gen.AddIndex(std::move(prop.index), std::move(prop.options));
    }
    return gen.Generate();
}

static void RunCHPreprocessing(Configuration&& cfg) {
    DatabaseHelper d{cfg.database.name, cfg.database.user, cfg.database.password, cfg.database.host, cfg.database.port};
    TableNameRepository name_rep{cfg.algorithm->base_graph_table, cfg.algorithm->name};
    for(auto&& profile : GenerateProfiles(d, cfg, 100)) {
        std::cout << "Load graph from " << cfg.algorithm->base_graph_table << "." << std::endl;
        using Graph = BidirectionalGraph<AdjacencyListGraph<CHVertex<CHPreprocessingEdge, VectorEdgeRange<CHPreprocessingEdge>>, CHPreprocessingEdge>>;
        Graph g{};
        database::UnpreprocessedDbGraph unpreprocessed_db_graph{};
        d.LoadFullGraph<Graph>(cfg.algorithm->base_graph_table, g, &unpreprocessed_db_graph);
        std::cout << "Profile: " << profile.GetName() << std::endl;
        profile.Set(g);
        std::cout << "Vertices: " << g.GetVertexCount() << std::endl;
        std::cout << "Edges before contraction: " << g.GetEdgeCount() << std::endl;
        std::string ch_edges_table{name_rep.GetEdgesTable(profile)};
        std::string ch_vertex_table{name_rep.GetVerticesTable(profile)};
        ContractionParameters parameters{g.GetMaxEdgeId() + 1, 5, 190, 120, 0};
        GraphContractor<Graph> c{g, parameters};
        c.ContractGraph();
        std::cout << "Contraction done." << std::endl;
        std::cout << "Edges after contraction: " << g.GetEdgeCount() << std::endl;
        database::CHDbGraph ch_db_graph{};
        d.CreateGraphTable(cfg.algorithm->base_graph_table, ch_edges_table, &ch_db_graph);
        d.DropGeographyIndex(ch_edges_table);
        std::cout << "Geography index dropped." << std::endl;
        d.AddShortcuts(ch_edges_table, g);
        std::cout << "Shortcuts added to " + ch_edges_table << "."<< std::endl;
        d.CreateGeographyIndex(ch_edges_table);
        std::cout << "Geography index restored." << std::endl;
        d.AddVertexOrdering(ch_vertex_table, g);
        std::cout << "Vertex ordering saved to " << ch_vertex_table << "." << std::endl;
    }
}

static void PrintHelp() {
    std::cout << "For algorithm preprocessing: --algorithm config_path.toml" << std::endl;
    std::cout << "For creating data indicies: --create-index indices_inputs.toml" << std::endl;
}