#ifndef BACKEND_ROUTING_CONFIGURATION_PARSER_H
#define BACKEND_ROUTING_CONFIGURATION_PARSER_H
#include "toml11/toml.hpp"
#include "routing/constants.h"
#include "routing/exception.h"
#include "routing/profile/data_index.h"
#include "routing/profile/green_index.h"
#include "routing/profile/physical_length_index.h"

#include <functional>
#include <iostream>
#include <unordered_map>
#include <memory>

namespace routing {

struct DatabaseConfig{
    std::string name;
    std::string user;
    std::string password;
    std::string host;
    std::string port;

    DatabaseConfig(const toml::value& table) :
        name(toml::find<std::string>(table, Constants::Input::Database::kName)),
        user(toml::find<std::string>(table, Constants::Input::Database::kUser)),
        password(toml::find<std::string>(table, Constants::Input::Database::kPassword)),
        host(toml::find<std::string>(table, Constants::Input::Database::kHost)),
        port(toml::find<std::string>(table, Constants::Input::Database::kPort))  {}
};

struct ProfileProperty {
    std::shared_ptr<profile::DataIndex> index;
    std::string table_name;
    std::vector<int32_t> options;

    ProfileProperty(std::shared_ptr<profile::DataIndex>&& i, std::string&& t, std::vector<int32_t>&& o) : index(std::move(i)), table_name(std::move(t)), options(std::move(o)) {}
};


struct AlgorithmConfig {
    std::string name;
    std::string base_graph_table;

    AlgorithmConfig(std::string&& n, std::string&& bgt) : name(std::move(n)), base_graph_table(std::move(bgt)) {}
};

struct CHConfig : public AlgorithmConfig {
    size_t hop_count;
    int32_t edge_difference_coefficient;
    int32_t deleted_neighbours_coefficient;
    int32_t space_size_coefficient;

    CHConfig(std::string&& n, std::string&& bgt, size_t hc, int32_t edc, int32_t dnc, int32_t ssc) 
        : AlgorithmConfig(std::move(n), std::move(bgt)), hop_count(hc), edge_difference_coefficient(edc), deleted_neighbours_coefficient(dnc),
        space_size_coefficient(ssc) {}
};

struct Configuration {
    DatabaseConfig database;
    std::vector<ProfileProperty> profile_properties;
    std::unique_ptr<AlgorithmConfig> algorithm;

    Configuration(DatabaseConfig&& db, std::vector<ProfileProperty>&& lm, std::unique_ptr<AlgorithmConfig> alg)
        : database(std::move(db)), profile_properties(std::move(lm)), algorithm(std::move(alg)) {}
};

class ConfigurationParser {
public:
    ConfigurationParser(const std::string& config_path);

    Configuration Parse();


private:
    const toml::value data_;

};

ConfigurationParser::ConfigurationParser(const std::string& config_path) : data_(toml::parse(config_path)) {}

Configuration ConfigurationParser::Parse() {
    auto&& database = toml::find(data_, Constants::Input::TableNames::kDatabase);
    std::string default_algorithm = Constants::AlgorithmNames::kContractionHierarchies;
    std::unordered_map<std::string, std::function<std::unique_ptr<AlgorithmConfig>(const toml::table&)>> algorithms {
        {Constants::AlgorithmNames::kContractionHierarchies, [](const toml::table& algorithm_config){
                std::string name = algorithm_config.at(Constants::Input::kName).as_string();
                std::string base_graph_table = algorithm_config.at(Constants::Input::kBaseGraphTable).as_string();
                auto&& param = algorithm_config.at(Constants::Input::TableNames::kParameters).as_table();

                return std::make_unique<CHConfig>(
                    std::move(name),
                    std::move(base_graph_table),
                    static_cast<size_t>(param.at(Constants::Input::Preprocessing::kHopCount).as_integer()),
                    static_cast<int32_t>(param.at(Constants::Input::Preprocessing::kEdgeDifference).as_integer()),
                    static_cast<int32_t>(param.at(Constants::Input::Preprocessing::kDeletedNeighbours).as_integer()),
                    static_cast<int32_t>(param.at(Constants::Input::Preprocessing::kSpaceSize).as_integer())
                );
            }
        }
    };
    
    DatabaseConfig db_config{database};

    auto&& algorithm = toml::find(data_, Constants::Input::TableNames::kAlgorithm);
    std::string alg_name = toml::find<std::string>(algorithm, Constants::Input::kName);
    if (!algorithms.contains(alg_name)) {
        alg_name = default_algorithm;
    } 
    std::unique_ptr<AlgorithmConfig> alg = algorithms[alg_name](algorithm.as_table());
    
    std::vector<ProfileProperty> profile_properties;
    for(auto&& property : toml::find<toml::array>(data_, Constants::Input::TableNames::kProfileProperties)) {
         std::unordered_map<std::string, std::function<std::shared_ptr<profile::DataIndex>()>> indicies{
            {Constants::IndexNames::kGreenIndex, [](){ return std::make_shared<profile::GreenIndex>(); }},
            {Constants::IndexNames::kLengthIndex, [](){ return std::make_shared<profile::PhysicalLengthIndex>(); }}
        };
        std::string name = toml::find<std::string>(property, Constants::Input::kName);
        if (!indicies.contains(name)) {
            throw ParseException{"Index " + name + " does not exist."};
        }
        std::string table_name = toml::find<std::string>(property, Constants::Input::kTableName);
        std::vector<int32_t> importance_options;
        for(auto&& importance : toml::find<toml::array>(property, Constants::Input::kImportance)) {
            importance_options.push_back(static_cast<int32_t>(importance.as_integer()));
        }
        profile_properties.emplace_back(indicies[name](), std::move(table_name), std::move(importance_options));
    }

    return Configuration{std::move(db_config), std::move(profile_properties), std::move(alg)};
}


}

#endif //BACKEND_ROUTING_CONFIGURATION_PARSER_H