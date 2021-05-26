#include "routing/edges/basic_edge.h"
#include "routing/table_name_repository.h"
#include "routing/exception.h"
#include "routing/configuration_parser.h"
#include "routing/constants.h"

#include "routing/preprocessing/algorithm_preprocessor.h"
#include "routing/preprocessing/ch_preprocessor.h"
#include "routing/preprocessing/contraction_parameters.h"

#include "routing/profile/profile_generator.h"
#include "routing/profile/profile.h"

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
static void CreateIndicies(const std::string& path);
static std::vector<profile::Profile> GenerateProfiles(DatabaseHelper& d, Configuration& cfg, double scale_max);
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
    auto&& profiles = GenerateProfiles(d, cfg, 100);
    std::unordered_map<std::string, std::function<std::unique_ptr<AlgorithmPreprocessor>(Configuration&&)>> algorithms{
        {Constants::AlgorithmNames::kContractionHierarchies, [&](Configuration&& cfg){
            CHConfig* ch_config = static_cast<CHConfig*>(cfg.algorithm.get());
            TableNameRepository name_rep{cfg.algorithm->base_graph_table, cfg.algorithm->name};
            ContractionParameters parameters{
                ch_config->hop_count,
                ch_config->edge_difference_coefficient,
                ch_config->deleted_neighbours_coefficient,
                ch_config->space_size_coefficient
            };
            return std::make_unique<CHPreprocessor>(std::move(d), std::move(name_rep), std::move(parameters));
        }}
    };
    std::unique_ptr<AlgorithmPreprocessor> alg = algorithms[cfg.algorithm->name](std::move(cfg));
    for(auto&& profile : profiles) {
        alg->RunPreprocessing(profile);
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

static std::vector<profile::Profile> GenerateProfiles(DatabaseHelper& d, Configuration& cfg, double scale_max) {
    ProfileGenerator gen{d, cfg.algorithm->base_graph_table, scale_max};
    for(auto&& prop : cfg.profile_properties) {
        prop.index->Load(d, prop.table_name);
        gen.AddIndex(std::move(prop.index), std::move(prop.options));
    }
    return gen.Generate();
}

static void PrintHelp() {
    std::cout << "For algorithm preprocessing: --algorithm config_path.toml" << std::endl;
    std::cout << "For creating data indicies: --create-index indices_inputs.toml" << std::endl;
}