#include "routing/profile/road_type_index.h"
#include "routing/exception.h"
#include "routing/constants.h"

#include "database/database_helper.h"


#include <string>
#include <vector>
#include <cassert>
#include <limits>
#include <unordered_map>


namespace routing {
namespace profile {

RoadTypeIndex::RoadTypeIndex() : impl_() {}

void RoadTypeIndex::Create(database::DatabaseHelper& d, const std::string& edges_table, const std::string& index_table) {
    std::unordered_map<std::string, double> road_type_values{
         {"primary", 0.8},
         {"secondary", 0.6},
         {"tertiary", 0.4},
         {"unclassified", 0.5},
         {"residential", 0.4},
         {"primary_link", 0.8},
         {"secondary_link", 0.6},
         {"tertiary_link", 0.4},
         {"living_street", 0.4},
         {"service", 0.4},
         {"pedestrian", 0.3},
         {"track", 0.1},
         {"footway", 0.1},
         {"bridleway", 0.2},
         {"path", 0.1},
         {"cycleway", 0.0},
         {"mini_roundabout", 0.5},
         {"crossing", 0.5},
         {"turning_loop", 0.5},
         {"turning_circle", 0.5}
    };

    std::vector<std::pair<unsigned_id_type, double>> index_values{};
    auto&& load = [&](const database::DbRow& row){
        unsigned_id_type uid = row.get<unsigned_id_type>(0);
        auto&& highway = row.get<std::string>(1);
        auto&& it = road_type_values.find(highway);
        double road_type_value = 0.5;
        if (it != road_type_values.end()) { 
            road_type_value = it->second;
        }
        index_values.emplace_back(uid, road_type_value);
    }; 
    std::string sql = 
            "SELECT uid, " + kValueColumnName + " "
            "FROM " + edges_table + "; ";
    d.RunNontransactional(sql, load);
    impl_.Create(d, index_values, index_table, kValueColumnName);
}

void RoadTypeIndex::Load(database::DatabaseHelper& d, const std::string& index_table) {
     std::string sql = 
            "SELECT uid, " + kValueColumnName + " "
            "FROM " + index_table + "; ";
    auto&& load = impl_.CreateLoadFunction();
    d.RunNontransactional(sql, load);
    Normalize();
}

void RoadTypeIndex::Create(database::DatabaseHelper& d, const std::vector<std::pair<unsigned_id_type, double>>& index_values,
        const std::string& index_table) const {
    impl_.Create(d, index_values, index_table, kValueColumnName);
}

double RoadTypeIndex::Get(unsigned_id_type uid) const {
    return impl_.Get(uid);
}

double RoadTypeIndex::GetOriginal(unsigned_id_type uid) const {
    return impl_.GetOriginal(uid);
}

const std::string& RoadTypeIndex::GetName() const {
    return Constants::IndexNames::kRoadTypeIndex;
}

void RoadTypeIndex::Normalize() {
    impl_.Normalize();
}





}
}