#include "routing/profile/peak_distance_index.h"
#include "routing/exception.h"
#include "routing/constants.h"

#include <string>
#include <vector>
#include <cassert>
#include <limits>

namespace routing {
namespace profile {

PeakDistanceIndex::PeakDistanceIndex() : impl_() {}

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
    auto&& load = impl_.CreateLoadFunction();
    d.RunNontransactional(sql, load);
    Normalize();
}

void PeakDistanceIndex::Create(database::DatabaseHelper& d, const std::vector<std::pair<unsigned_id_type, double>>& index_values, const std::string& index_table) const {
    impl_.Create(d, index_values, index_table, kValueColumnName);
}

double PeakDistanceIndex::Get(unsigned_id_type uid) const {
    return impl_.Get(uid);
}

const std::string& PeakDistanceIndex::GetName() const {
    return Constants::IndexNames::kPeakDistanceIndex;
}

void PeakDistanceIndex::Normalize() {
    impl_.Normalize(kScaleMax);
}





}
}