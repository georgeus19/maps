#include "routing/edges/basic_edge.h"
#include "routing/table_names.h"
#include "routing/exception.h"
#include "routing/configuration_parser.h"
#include "routing/constants.h"

#include "routing/preprocessing/algorithm_preprocessor.h"
#include "routing/preprocessing/ch_preprocessor.h"
#include "routing/preprocessing/contraction_parameters.h"
#include "routing/preprocessing/index_extender.h"

#include "routing/profile/profile_generator.h"
#include "routing/profile/profile.h"
#include "routing/profile/green_index.h"
#include "routing/profile/physical_length_index.h"
#include "routing/profile/peak_distance_index.h"

#include "database/database_helper.h"

#include "toml11/toml.hpp"

#include <chrono>
#include <vector>
#include <tuple>
#include <string>
#include <memory>
#include <unordered_map>
#include <iostream>

using namespace std;
using namespace database;
using namespace routing;
using namespace preprocessing;
using namespace profile;

static void RunAlgorithmPreprocessing(const std::string& config_path);
template <typename Graph>
static void ExtendProfileIndicies(Configuration& cfg, Graph& graph, Profile& profile, TableNames* table_names, DatabaseHelper& d);
static void CreateIndicies(const std::string& path);
static std::vector<profile::Profile> GenerateProfiles(DatabaseHelper& d, Configuration& cfg);
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
            CreateIndicies(argv[2]);
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
    auto&& profiles = GenerateProfiles(d, cfg);
    std::unordered_map<std::string, std::function<void(Configuration&, Profile& profile)>> algorithms{
        {Constants::AlgorithmNames::kContractionHierarchies, [&](Configuration& cfg, Profile& profile){
            CHConfig* ch_config = static_cast<CHConfig*>(cfg.algorithm.get());
            ContractionParameters parameters{
                ch_config->hop_count,
                ch_config->edge_difference_coefficient,
                ch_config->deleted_neighbours_coefficient,
                ch_config->space_size_coefficient
            };
            CHTableNames table_names{cfg.algorithm->base_graph_table, profile, cfg.algorithm->mode};
            CHPreprocessor preprocessor{d, &table_names, std::move(parameters)};
            auto&& graph = preprocessor.LoadGraph(profile);
            preprocessor.RunPreprocessing(graph);
            if (cfg.algorithm->mode == Constants::ModeNames::kDynamicProfile) {
                ExtendProfileIndicies<CHPreprocessor::Graph>(cfg, graph, profile, &table_names, d);
            }
            preprocessor.SaveGraph(graph);
        }}
    };
    for(auto&& profile : profiles) {
        auto it = algorithms.find(cfg.algorithm->name);
        if (it != algorithms.end()) {
            it->second(cfg, profile);
        } else {
            throw InvalidArgumentException{"There is no preprocessing for algorithm " + cfg.algorithm->name + "."};
        }
    }
}

template <typename Graph>
static void ExtendProfileIndicies(Configuration& cfg, Graph& graph, Profile& profile, TableNames* table_names, DatabaseHelper& d) {
    IndexExtender<Graph> extender{d, graph};
    for(auto&& prop : cfg.profile_properties) {
        std::shared_ptr<DataIndex> index =  profile.GetIndex(prop.index->GetName());
        if (!index) {
            throw InvalidValueException{"Index " + prop.table_name + " not in profile when up for extension."};
        }
        std::string new_index_table = table_names->GetIndexTablePrefix() + prop.table_name;
        std::cout << "Extend index " << prop.table_name << " to table " << new_index_table << std::endl;
        extender.ExtendIndex(index, new_index_table);
    }
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
        {
            Constants::IndexNames::kPeakDistanceIndex,
            [&](const toml::value& table){
                PeakDistanceIndex index{};
                index.Create(d, 
                    toml::find<std::string>(table, Constants::Input::Indicies::kEdgesTable),
                    toml::find<std::string>(table, Constants::Input::Indicies::kPointTable),
                    toml::find<std::string>(table, Constants::Input::Indicies::kIndexTable)
                ); 
            }
        },
        {Constants::IndexNames::kLengthIndex, [](const toml::value&){ /* no action necessary... - already present in base graph! */ }}
    };
    for(auto&& index : toml::find<toml::array>(data, Constants::Input::TableNames::kIndicies)) {
        std::string name = toml::find<std::string>(index, Constants::Input::kName);
        auto&& it = indicies.find(name);
        if (it != indicies.end()) {
            it->second(index);
            std::cout << "Index " << name << " created." << std::endl;
        } else {
            std::cout << "No index " << name << " exists." << std::endl;
        }
    }
}

static std::vector<profile::Profile> GenerateProfiles(DatabaseHelper& d, Configuration& cfg) {
    ProfileGenerator gen{d, cfg.algorithm->base_graph_table};
    for(auto&& prop : cfg.profile_properties) {
        std::cout << "Loading index from " << prop.table_name << std::endl;
        prop.index->Load(d, prop.table_name);
        gen.AddIndex(prop.index, std::move(prop.options));
    }
    return gen.Generate();
}

static void PrintHelp() {
    std::cout << "For algorithm preprocessing: --algorithm config_path.toml" << std::endl;
    std::cout << "For creating data indicies: --create-index indices_inputs.toml" << std::endl;
}