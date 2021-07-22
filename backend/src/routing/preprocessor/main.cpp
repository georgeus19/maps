#include "routing/edges/basic_edge.h"
#include "routing/types.h"
#include "routing/table_names.h"
#include "routing/exception.h"
#include "routing/configuration_parser.h"
#include "routing/constants.h"

#include "routing/preprocessing/ch_preprocessor.h"
#include "routing/preprocessing/contraction_parameters.h"
#include "routing/preprocessing/index_extender.h"

#include "routing/profile/profile_generator.h"
#include "routing/profile/profile.h"
#include "routing/profile/green_index.h"
#include "routing/profile/physical_length_index.h"
#include "routing/profile/peak_distance_index.h"
#include "routing/profile/road_type_index.h"

#include "routing/database/database_helper.h"

#include "toml11/toml.hpp"

#include <chrono>
#include <vector>
#include <tuple>
#include <string>
#include <memory>
#include <unordered_map>
#include <iostream>

using namespace std;
using namespace routing;
using namespace database;
using namespace preprocessing;
using namespace profile;

/**
 * Main entrypoint for any algorithm preprocessing.
 */
static void RunAlgorithmPreprocessing(const std::string& config_path);
/**
 * Run preprocessing for StaticProfileMode based on the configuration.
 */
static void StaticModePreprocessing(DatabaseHelper& d, Configuration& cfg);

/**
 * Run preprocessing for DynamicProfileMode based on the configuration.
 */
static void DynamicModePreprocessing(DatabaseHelper& d, Configuration& cfg);

/**
 * Preprocessing of routing algorithms such as CH create additional edges. Compute preference values for these new
 * edges and save them along with the old one to database table.
 */
template <typename Graph>
static void ExtendProfileIndices(Configuration& cfg, Graph& graph, Profile& profile, TableNames* table_names, DatabaseHelper& d);

/**
 * Main entrypoint for creating preference indices.
 */
static void CreateIndices(const std::string& path);

static void PrintHelp();

// Run options.
const std::string kCreateIndex = "--create-index";
const std::string kAlgorithmPreprocessing = "--algorithm";

int main(int argc, const char** argv) {
    if (argc != 3) {
        PrintHelp();
        return 1;
    }

    std::unordered_map<std::string, std::function<void()>> run_options{
        {kCreateIndex, [=](){
            CreateIndices(argv[2]);
        }},
        {kAlgorithmPreprocessing, [=](){
            RunAlgorithmPreprocessing(argv[2]);
        }}
    };

    try {
        auto&& it = run_options.find(argv[1]);
        if (it != run_options.end()) {
            it->second();
        } else {
            throw InvalidArgumentException("Invalid first argument - no " + std::string{argv[1]} + " program run option found.");
        }    
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        PrintHelp();
        return 1;
    }
    return 0;
}

static void RunAlgorithmPreprocessing(const std::string& config_path) {
    ConfigurationParser parser{config_path};
    auto&& cfg = parser.Parse();
    DatabaseHelper d{cfg.database.name, cfg.database.user, cfg.database.password, cfg.database.host, cfg.database.port};

    std::unordered_map<std::string, std::function<void(DatabaseHelper& d, Configuration& cfg)>> modes{
        {Constants::ModeNames::kStaticProfile, StaticModePreprocessing},
        {Constants::ModeNames::kDynamicProfile, DynamicModePreprocessing}
    };

    auto it = modes.find(cfg.algorithm->mode);
    if (it != modes.end()) {
        it->second(d, cfg);
    } else {
        throw InvalidArgumentException{"No mode " + cfg.algorithm->mode + " found!"};
    }
}

static void CHPreprocessing(DatabaseHelper&d, Configuration& cfg, Profile& profile) {
    CHConfig* ch_config = static_cast<CHConfig*>(cfg.algorithm.get());
    ContractionParameters parameters{
        ch_config->hop_count,
        ch_config->edge_difference_coefficient,
        ch_config->deleted_neighbours_coefficient,
        ch_config->space_size_coefficient
    };
    CHTableNames table_names{cfg.algorithm->base_graph_table, profile};
    CHPreprocessor preprocessor{d, &table_names, std::move(parameters)};
    auto&& graph = preprocessor.LoadGraph(profile);
    preprocessor.RunPreprocessing(graph);
    if (cfg.algorithm->mode == Constants::ModeNames::kDynamicProfile) {
        ExtendProfileIndices<CHPreprocessor::Graph>(cfg, graph, profile, &table_names, d);
    }
    preprocessor.SaveGraph(graph);
}

static void StaticModePreprocessing(DatabaseHelper& d, Configuration& cfg) {
    std::unordered_map<std::string, std::function<void(DatabaseHelper&, Configuration&, Profile&)>> algorithms{
        {Constants::AlgorithmNames::kContractionHierarchies, CHPreprocessing}
    };
    cfg.profile_preferences.LoadIndices(d);
    auto&& gen = cfg.profile_preferences.GetProfileGenerator();
    for(auto&& profile : gen.Generate()) {
        auto it = algorithms.find(cfg.algorithm->name);
        if (it != algorithms.end()) {
            it->second(d, cfg, profile);
        } else {
            throw InvalidArgumentException{"There is no preprocessing for algorithm " + cfg.algorithm->name + "."};
        }
    }
}

static void DynamicModePreprocessing(DatabaseHelper& d, Configuration& cfg) {
    std::unordered_map<std::string, std::function<void(DatabaseHelper&, Configuration&, Profile&)>> algorithms{
        {Constants::AlgorithmNames::kContractionHierarchies, CHPreprocessing}
    };
    cfg.profile_preferences.LoadIndices(d);
    auto&& gen = cfg.profile_preferences.GetProfileGenerator();
    Profile profile = gen.GetFrontProfile();

    auto it = algorithms.find(cfg.algorithm->name);
    if (it != algorithms.end()) {
        it->second(d, cfg, profile);
    } else {
        throw InvalidArgumentException{"There is no preprocessing for algorithm " + cfg.algorithm->name + "."};
    }
}

template <typename Graph>
static void ExtendProfileIndices(Configuration& cfg, Graph& graph, Profile& profile, TableNames* table_names, DatabaseHelper& d) {
    IndexExtender<Graph> extender{d, graph};
    auto&& base_index = profile.GetBaseIndex();
    std::string new_base_index_table = table_names->GetIndexTablePrefix() + cfg.profile_preferences.base_index_table;
    std::cout << "Extend base index " << cfg.profile_preferences.base_index_table << " to table " << new_base_index_table << std::endl;
    extender.ExtendIndex(base_index, new_base_index_table);
    
    for(auto&& prop : cfg.profile_preferences.properties) {
        auto&& index =  profile.GetIndex(prop.index->GetName());
        std::string new_index_table = table_names->GetIndexTablePrefix() + prop.table_name;
        std::cout << "Extend index " << prop.table_name << " to table " << new_index_table << std::endl;
        extender.ExtendIndex(index, new_index_table);
    }
}

static void CreateIndices(const std::string& path) {
    std::cout << "If you kill this process, restart PostgreSQL database." << std::endl;
    auto&& data = toml::parse(path);
    DatabaseConfig db_cfg{toml::find(data, Constants::Input::TableNames::kDatabase)};
    DatabaseHelper d{db_cfg.name, db_cfg.user, db_cfg.password, db_cfg.host, db_cfg.port};
    std::unordered_map<std::string, std::function<void(const toml::value&)>> indices{
        {
            Constants::IndexNames::kGreenIndex,
            [&](const toml::value& table){
                GreenIndex index{};
                index.Create(d, 
                    toml::find<std::string>(table, Constants::Input::Indices::kEdgesTable),
                    toml::find<std::string>(table, Constants::Input::Indices::kPolygonTable),
                    toml::find<std::string>(table, Constants::Input::Indices::kIndexTable)
                ); 
            }
        },
        {
            Constants::IndexNames::kPeakDistanceIndex,
            [&](const toml::value& table){
                PeakDistanceIndex index{};
                index.Create(d, 
                    toml::find<std::string>(table, Constants::Input::Indices::kEdgesTable),
                    toml::find<std::string>(table, Constants::Input::Indices::kPointTable),
                    toml::find<std::string>(table, Constants::Input::Indices::kIndexTable)
                ); 
            }
        },
        {
            Constants::IndexNames::kLengthIndex,
            [&](const toml::value& table){
                PhysicalLengthIndex index{};
                index.Create(d,
                    toml::find<std::string>(table, Constants::Input::Indices::kEdgesTable),
                    toml::find<std::string>(table, Constants::Input::Indices::kIndexTable)
                );
            }
        },
         {
            Constants::IndexNames::kRoadTypeIndex,
            [&](const toml::value& table){
                RoadTypeIndex index{};
                index.Create(d,
                    toml::find<std::string>(table, Constants::Input::Indices::kEdgesTable),
                    toml::find<std::string>(table, Constants::Input::Indices::kIndexTable)
                );
            }
        }
    };
    for(auto&& index : toml::find<toml::array>(data, Constants::Input::TableNames::kIndices)) {
        std::string name = toml::find<std::string>(index, Constants::Input::kName);
        auto&& it = indices.find(name);
        if (it != indices.end()) {
            it->second(index);
            std::cout << "Index " << name << " created." << std::endl;
        } else {
            std::cout << "No index " << name << " exists." << std::endl;
        }
    }
}

static void PrintHelp() {
    std::cout << "For algorithm preprocessing: --algorithm config_path.toml" << std::endl;
    std::cout << "For creating data indices: --create-index indices_inputs.toml" << std::endl;
}