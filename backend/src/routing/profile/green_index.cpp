#include "routing/profile/green_index.h"
#include "routing/exception.h"
#include "routing/constants.h"

#include <string>
#include <vector>
#include <cassert>
#include <limits>

namespace routing {
namespace profile {

GreenIndex::GreenIndex() : impl_() {}

void GreenIndex::Create(database::DatabaseHelper& d, const std::string& edges_table, const std::string& osm_polygons_table, const std::string& green_index_table) {
    std::string green_places_table = osm_polygons_table + "_green_geom_temp"; 
	std::string green_places_table_index = green_places_table + "_gix";
	std::string green_index_table_index = green_index_table + "_idx";
	std::string sql = 
			"DROP INDEX IF EXISTS " + green_index_table_index  + "; "
			"DROP TABLE IF EXISTS " + green_index_table + "; " 
			"CREATE TABLE " + green_index_table + " AS  "
			"SELECT "
			"	edges.uid, "
			"    SUM( "
			"        ST_Area(ST_Intersection(edges.geom, green.way)) / ST_Area(edges.geom) "
			"    ) AS " + kValueColumnName + " "
			"FROM  "
			"( "
			"	SELECT uid, ST_Buffer(ST_Transform(geog::geometry, 3857), 30) as geom "
			"	FROM " + edges_table + " "
			") AS edges LEFT JOIN "
			"( "
			"	SELECT p.way "
			"	FROM " + osm_polygons_table + " AS p "
			"	WHERE p.natural IN ('wood', 'tree_row', 'tree', 'scrub', 'heath', 'moor', 'grassland', 'fell', 'tundra', "
			"			'bare_rock', 'scree', 'shingle', 'sand', 'mud', 'water', 'wetland', 'glacier', 'bay', 'cape', 'strait', 'beach', 'coastline', "
			"			'reef', 'spring', 'hot_spring', 'geyser', 'peak', 'dune', 'hill', 'volcano', 'valley', 'ridge', 'arete', 'cliff', 'saddle', "
			"			'isthmus', 'peninsula', 'rock', 'stone', 'sinkhole', 'cave_entrance') "
			"		OR p.landuse IN ('farmland', 'forest', 'meadow', 'orchard', 'vineyard', 'basin', 'grass', 'plant_nursery', 'village_green',"
			"			'recreation_ground', 'allotments', 'cemetery') "
            "   	OR p.leisure in ('disc_golf_course', 'dog_park', 'golf_course', 'garden', 'park', 'pitch', 'beach_resort') "
			") as green ON edges.geom && green.way "
			"GROUP BY edges.uid; "
			"CREATE UNIQUE INDEX " + green_index_table_index  + " ON " + green_index_table + " (uid); "; 
    d.RunTransactional(sql);
}

void GreenIndex::Load(database::DatabaseHelper& d, const std::string& green_index_table) {
    // edge_green_values_.assign(max_uid + 1, GreenValue{});
    std::string sql = 
            "SELECT uid, COALESCE(" + kValueColumnName + ", 0) "
            "FROM " + green_index_table + "; ";
    auto&& load = impl_.CreateLoadFunction();
    d.RunNontransactional(sql, load);
    Normalize();
}

void GreenIndex::Create(database::DatabaseHelper& d, const std::vector<std::pair<unsigned_id_type, double>>& index_values,
		const std::string& index_table) const {
    impl_.Create(d, index_values, index_table, kValueColumnName);
}


double GreenIndex::Get(unsigned_id_type uid) const {
    return impl_.Get(uid);
}

const std::string& GreenIndex::GetName() const {
    return Constants::IndexNames::kGreenIndex;
}

void GreenIndex::Normalize() {
    impl_.ForEachValue([](PairIndexImplementation::Value& value){
        if (value.valid) {
            if (value.value > 1) {
                value.value = 1;
            }
            
            // The lower, the better green index in routing so it is necessary to flip it.
            // The create query gives green_value the higher, the better.
            value.value = 1 - value.value;

            // The value should be within [0, scale_max].
            value.value *= kScaleMax;
        }
    });
}




}
}