#ifndef ROUTING_CONFIGURATION_PARSER_H
#define ROUTING_CONFIGURATION_PARSER_H
#include "toml11/toml.hpp"
#include "routing/constants.h"
#include "routing/exception.h"
#include "routing/types.h"

#include "routing/profile/preference_index.h"
#include "routing/profile/green_index.h"
#include "routing/profile/physical_length_index.h"
#include "routing/profile/peak_distance_index.h"
#include "routing/profile/road_type_index.h"
#include "routing/profile/profile_generator.h"

#include "routing/database/database_helper.h"

#include <functional>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>

/**
 * The whole configuration data is loaded to matching classes before any special events are done.
 */ 

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

struct ProfilePreference {
    std::shared_ptr<profile::PreferenceIndex> index;
    std::string table_name;
    std::vector<float> options;
    std::string display_name;
    std::vector<std::string> option_display_names;


    ProfilePreference(std::shared_ptr<profile::PreferenceIndex>&& i, std::string&& t, std::vector<float>&& o,
        std::string&& dn, std::vector<std::string>&& odn)
        : index(std::move(i)), table_name(std::move(t)), options(std::move(o)), display_name(std::move(dn)), option_display_names(std::move(odn)) {}
};


struct AlgorithmConfig {
    std::string name;
    std::string base_graph_table;
    std::string mode;

    AlgorithmConfig(std::string&& n, std::string&& bgt, std::string&& m) : name(std::move(n)), base_graph_table(std::move(bgt)), mode(std::move(m)) {}
};

struct CHConfig : public AlgorithmConfig {
    size_t hop_count;
    int32_t edge_difference_coefficient;
    int32_t deleted_neighbours_coefficient;
    int32_t space_size_coefficient;

    CHConfig(std::string&& n, std::string&& bgt, std::string&&m, size_t hc, int32_t edc, int32_t dnc, int32_t ssc) 
        : AlgorithmConfig(std::move(n), std::move(bgt), std::move(m)), hop_count(hc), edge_difference_coefficient(edc), deleted_neighbours_coefficient(dnc),
        space_size_coefficient(ssc) {}
};

struct ProfilePreferences{
    std::shared_ptr<profile::PreferenceIndex> base_index;
    std::string base_index_table;
    std::vector<ProfilePreference> properties;

    ProfilePreferences(std::shared_ptr<profile::PreferenceIndex>&& bi, std::string&& bit, std::vector<ProfilePreference>&& pp)
        : base_index(std::move(bi)), base_index_table(std::move(bit)), properties(std::move(pp)) {}

    profile::ProfileGenerator GetProfileGenerator() {
        profile::ProfileGenerator gen{base_index};
        for(auto&& prop : properties) {
            gen.AddIndex(prop.index, std::move(prop.options));
        }
        return gen;
    }

    void LoadIndices(database::DatabaseHelper& d, const std::string& table_name_prefix = std::string{}) {
        std::string base_index_complete_table = table_name_prefix + base_index_table;
        std::cout << "Load base index from " << base_index_complete_table << std::endl;
        base_index->Load(d, base_index_complete_table);
        for(auto&& prop : properties) {
            std::string index_table = table_name_prefix + prop.table_name;
            std::cout << "Load index from " << index_table << std::endl;
            prop.index->Load(d, index_table);
        }
    }
};

struct Configuration {
    DatabaseConfig database;
    ProfilePreferences profile_preferences;
    std::unique_ptr<AlgorithmConfig> algorithm;

    Configuration(DatabaseConfig&& db, ProfilePreferences&& pp, std::unique_ptr<AlgorithmConfig> alg)
        : database(std::move(db)), profile_preferences(std::move(pp)), algorithm(std::move(alg)) {}
};

class ConfigurationParser {
public:
    ConfigurationParser(const std::string& config_path);

    /**
     * Parse configuration file and return a class representing everything in the configuration file.
     */
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
                std::string mode = algorithm_config.at(Constants::Input::kMode).as_string();
                auto&& param = algorithm_config.at(Constants::Input::TableNames::kParameters).as_table();

                return std::make_unique<CHConfig>(
                    std::move(name),
                    std::move(base_graph_table),
                    std::move(mode),
                    static_cast<size_t>(param.at(Constants::Input::Preprocessing::kHopCount).as_integer()),
                    static_cast<int32_t>(param.at(Constants::Input::Preprocessing::kEdgeDifference).as_integer()),
                    static_cast<int32_t>(param.at(Constants::Input::Preprocessing::kDeletedNeighbours).as_integer()),
                    static_cast<int32_t>(param.at(Constants::Input::Preprocessing::kSpaceSize).as_integer())
                );
            }
        },
        {Constants::AlgorithmNames::kDijkstra, [](const toml::table& algorithm_config){
                std::string name = algorithm_config.at(Constants::Input::kName).as_string();
                std::string base_graph_table = algorithm_config.at(Constants::Input::kBaseGraphTable).as_string();
                std::string mode = algorithm_config.at(Constants::Input::kMode).as_string();

                return std::make_unique<AlgorithmConfig>(
                    std::move(name),
                    std::move(base_graph_table),
                    std::move(mode)
                );
            }
        }
    };
    
    DatabaseConfig db_config{database};

    auto&& algorithm = toml::find(data_, Constants::Input::TableNames::kAlgorithm);
    std::string alg_name = toml::find<std::string>(algorithm, Constants::Input::kName);
    if (algorithms.find(alg_name) == algorithms.end()) {
        throw ParseException{"Algorithm " + alg_name + " does not exist."};
    } 
    std::unique_ptr<AlgorithmConfig> alg = algorithms[alg_name](algorithm.as_table());
    
    std::unordered_map<std::string, std::function<std::shared_ptr<profile::PreferenceIndex>()>> indices{
        {Constants::IndexNames::kGreenIndex, [](){ return std::make_shared<profile::GreenIndex>(); }},
        {Constants::IndexNames::kPeakDistanceIndex, [](){ return std::make_shared<profile::PeakDistanceIndex>(); }},
        {Constants::IndexNames::kLengthIndex, [](){ return std::make_shared<profile::PhysicalLengthIndex>(); }},
        {Constants::IndexNames::kRoadTypeIndex, [](){ return std::make_shared<profile::RoadTypeIndex>(); }}
    };

    auto&& preferences = toml::find(data_, Constants::Input::TableNames::kPreferences);
    std::string base_index = toml::find<std::string>(preferences, Constants::Input::kBaseIndex);
    if (indices.find(base_index) == indices.end()) {
        throw ParseException{"Base index " + base_index + " does not exist."};
    }
    std::string base_index_table = toml::find<std::string>(preferences, Constants::Input::kBaseIndexTable);
    
    std::vector<ProfilePreference> profile_preferences;
    for(auto&& index : toml::find<toml::array>(preferences, Constants::Input::TableNames::kIndices)) {
        
        std::string name = toml::find<std::string>(index, Constants::Input::kName);
        if (indices.find(name) == indices.end()) {
            throw ParseException{"Index " + name + " does not exist."};
        }
        std::string table_name = toml::find<std::string>(index, Constants::Input::kTableName);
        std::vector<float> importance_options;
        for(auto&& importance : toml::find<toml::array>(index, Constants::Input::kImportance)) {
            importance_options.push_back(static_cast<float>(importance.as_floating()));
        }
        
        std::string display_name = toml::find<std::string>(index, Constants::Input::kDisplayName);
        std::vector<std::string> importance_options_display_names;
        for(auto&& importance_display_name : toml::find<toml::array>(index, Constants::Input::kDisplayImportance)) {
            importance_options_display_names.push_back(static_cast<std::string>(importance_display_name.as_string()));
        }

        profile_preferences.emplace_back(indices[name](), std::move(table_name), std::move(importance_options), std::move(display_name),
            std::move(importance_options_display_names));
    }

    ProfilePreferences pref{ 
        indices[base_index](),
        std::move(base_index_table),
        std::move(profile_preferences)
    };

    return Configuration{std::move(db_config), std::move(pref), std::move(alg)};
}



}
#endif //ROUTING_CONFIGURATION_PARSER_H