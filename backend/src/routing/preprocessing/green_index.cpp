#include "routing/preprocessing/green_index.h"
#include "routing/exception.h"

#include <string>
#include <vector>
#include <cassert>
#include <limits>

namespace routing {
namespace preprocessing {

GreenIndex::GreenIndex(database::DatabaseHelper& d) : d_(d), edge_green_values_() {}

void GreenIndex::Create(const std::string& edges_table, const std::string& osm_polygons_table, const std::string& green_index_table) {
    std::string green_places_table = osm_polygons_table + "_green_geom_temp"; 
	std::string green_places_table_index = green_places_table + "_gix";
	std::string green_index_table_index = green_index_table + "_idx";
	std::string sql = 
			"DROP INDEX IF EXISTS " + green_index_table_index  + "; "
			"DROP TABLE IF EXISTS " + green_index_table + "; " 
			"DROP TABLE IF EXISTS " + green_places_table + "; "
			"CREATE TEMPORARY TABLE " + green_places_table + " AS "
			"SELECT p.way "
			"FROM " + osm_polygons_table + " AS p "
			"WHERE p.natural IN ('wood', 'tree_row', 'tree', 'scrub', 'heath', 'moor', 'grassland', 'fell', 'tundra', "
			"	'bare_rock', 'scree', 'shingle', 'sand', 'mud', 'water', 'wetland', 'glacier', 'bay', 'cape', 'strait', 'beach', 'coastline', "
			"	'reef', 'spring', 'hot_spring', 'geyser', 'peak', 'dune', 'hill', 'volcano', 'valley', 'ridge', 'arete', 'cliff', 'saddle', "
			"	'isthmus', 'peninsula', 'rock', 'stone', 'sinkhole', 'cave_entrance') "
			"	OR p.landuse IN ('farmland', 'forest', 'meadow', 'orchard', 'vineyard', 'basin', 'grass', 'village_green'); "
			"CREATE INDEX " + green_places_table_index + " ON " + green_places_table + " USING GIST (way); "
			"CREATE TABLE " + green_index_table + " AS  "
			"SELECT "
			"	edges.uid, "
			"    SUM( "
			"        ST_Area(ST_Intersection(edges.geom, green.way)) / ST_Area(edges.geom) "
			"    ) AS green_fraction "
			"FROM  "
			"( "
			"	SELECT uid, ST_Buffer(ST_Transform(geog::geometry, 3857), 30) as geom "
			"	FROM " + edges_table + " "
			") AS edges LEFT JOIN " + green_places_table + " as green ON edges.geom && green.way "
			"GROUP BY edges.uid; "
			"DROP INDEX IF EXISTS " + green_places_table_index + "; "
			"CREATE UNIQUE INDEX " + green_index_table_index  + " ON " + green_index_table + " (uid); "; 
    d_.RunTransactional(sql);
}

void GreenIndex::Load(const std::string& green_index_table, size_t max_uid) {
    edge_green_values_.assign(max_uid + 1, GreenValue{});
    std::string sql = 
            "SELECT uid, COALESCE(green_fraction, 0) "
            "FROM " + green_index_table + "; ";
    auto&& load = [&](const database::DbRow& row) {
        unsigned_id_type uid = row.get<unsigned_id_type>(0);
        double green_value = row.get<double>(1);
        edge_green_values_[uid] = green_value;
    };
    d_.RunNontransactional(sql, load);
}

void GreenIndex::Normalize(double scale_max) {
    double max_value = std::numeric_limits<double>::min();
    for(auto&& green_value : edge_green_values_) {
        if (green_value.valid) {
            if (max_value < green_value.value) {
                max_value = green_value.value;
            }
        }
    }
    for(auto&& green_value : edge_green_values_) {
        if (green_value.valid) {
            green_value.value /= max_value;
            green_value.value *= scale_max;
        }
    }
}

double GreenIndex::Get(unsigned_id_type uid) {
    assert(uid < edge_green_values_.size());
    GreenValue gv = edge_green_values_[uid];
    if (gv.valid) {
        return gv.value;
    } else {
        throw InvalidValueException{"Green value of edge " + std::to_string(uid) + " not found - resp. invalid value is in its place."};
    }
}


}
}