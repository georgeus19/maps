#include "routing/profile/peak_distance_index.h"
#include "routing/exception.h"
#include "routing/constants.h"

#include <string>
#include <vector>
#include <cassert>
#include <limits>

namespace routing {
namespace profile {

PeakDistanceIndex::PeakDistanceIndex() : edge_peak_distances_() {}

void PeakDistanceIndex::Create(database::DatabaseHelper& d, const std::string& edges_table,
    const std::string& osm_point_table, const std::string& peak_index_table) {
    std::string sql = 
            "DROP TABLE IF EXISTS " + peak_index_table + "; "
            "CREATE TABLE " + peak_index_table + " AS "
            "SELECT e.uid, SUM( "
            "    " + std::to_string(kDistanceRadius) + " - ST_Distance(e.original_geom, p.geom) "
            ") AS " + kValueColumnName + " "
            "FROM (SELECT uid, "
            "ST_Buffer(ST_Transform(geog::geometry, 3857), " + std::to_string(kDistanceRadius) + ") AS geom, "
            "ST_Transform(geog::geometry, 3857) as original_geom "
            "FROM " + edges_table + ") AS e "
            "LEFT JOIN  "
            "( "
            "    SELECT pom.way as geom "
            "    FROM " + osm_point_table + " as pom "
            "    WHERE pom.natural = 'peak' "
            ") AS p ON (e.geom && p.geom and ST_Distance(e.original_geom, p.geom) < " + std::to_string(kDistanceRadius) + ")"
            "GROUP BY e.uid";
    d.RunTransactional(sql);
}

void PeakDistanceIndex::Load(database::DatabaseHelper& d, const std::string& peak_index_table) {
    // edge_peak_distances_.assign(max_uid + 1, GreenValue{});
    double max_distance = kDistanceRadius * 1.5;
    std::string sql = 
            "SELECT uid, LEAST(COALESCE(" + kValueColumnName + ", " + std::to_string(max_distance) + "), " + std::to_string(max_distance) + ") "
            "FROM " + peak_index_table + "; ";
    auto&& load = [&](const database::DbRow& row) {
        unsigned_id_type uid = row.get<unsigned_id_type>(0);
        double green_value = row.get<double>(1);
        if (uid >= edge_peak_distances_.size()) {
            edge_peak_distances_.resize(uid + 1);
        }
        edge_peak_distances_[uid] = green_value;
    };
    d.RunNontransactional(sql, load);
}

void PeakDistanceIndex::Create(database::DatabaseHelper& d, const std::vector<std::pair<unsigned_id_type, double>>& index_values, const std::string& index_table) const {
    PairIndexImplementation{}.Create(d, index_values, index_table, kValueColumnName);
}

void PeakDistanceIndex::Normalize(double scale_max) {
    double max_value = std::numeric_limits<double>::min();
    for(auto&& peak_value : edge_peak_distances_) {
        if (peak_value.valid) {
            if (max_value < peak_value.peak_distance) {
                max_value = peak_value.peak_distance;
            }
        }
    }
    for(auto&& peak_value : edge_peak_distances_) {
        if (peak_value.valid) {
            peak_value.peak_distance /= max_value;
            peak_value.peak_distance *= scale_max;
        }
    }
}

double PeakDistanceIndex::Get(unsigned_id_type uid) const {
    assert(uid < edge_peak_distances_.size());
    PeakValue pv = edge_peak_distances_[uid];
    if (pv.valid) {
        return pv.peak_distance;
    } else {
        throw InvalidValueException{"Peak distance value of edge " + std::to_string(uid) + " not found - resp. invalid value is in its place."};
    }
}

const std::string& PeakDistanceIndex::GetName() const {
    return Constants::IndexNames::kPeakDistanceIndex;
}


}
}