#include "database/database_helper.h"

using namespace std;
namespace database {

DatabaseHelper::DatabaseHelper(const string & db_name, const string & user, const string & password,
								const string & host_address, const string & port) : db_name_(db_name),
								user_(user), password_(password), host_address_(host_address),
								port_(port), connection_("dbname = " + db_name_ + " user = " + user_ +
														" password = " + password_ + " hostaddr = " + host_address_ +
														" port = " + port_) {}

DatabaseHelper::~DatabaseHelper() {
	connection_.disconnect();
}

bool DatabaseHelper::IsDbOpen() {
	return connection_.is_open();
}

/*
	* Example of sql:
-- Get the closest graph edge to POINT(13.394182 49.725673).
WITH closest_candidates AS (
SELECT e.osm_id, e.uid, e.geog, e.from_node, e.to_node, e.length
FROM czedges as e
ORDER BY
e.geog <-> 'SRID=4326;POINT(13.394182 49.725673)'::geography
LIMIT 100
),
closest_edge AS (SELECT *
FROM closest_candidates
ORDER BY
ST_Distance(geog, 'SRID=4326;POINT(13.394182 49.725673)'::geography)
LIMIT 1
)
SELECT *, 'SRID=4326;POINT(13.394182 49.725673)'::geography, st_asText(geog),
st_astext(st_transform(
st_closestpoint(
	st_transform(geog::geometry, 3785),
	st_transform('SRID=4326;POINT(13.394182 49.725673)'::geometry, 3785)
),
4326))
FROM closest_edge
	*/
std::unique_ptr<DbEdgeIterator> DatabaseHelper::FindClosestEdge(utility::Point p, const string & table_name, DbGraph* db_graph){
	string point = MakeGeographyPoint(p);
	/*
		* Find closest 100 candidates to be the closest edge by comparing bounding rectangles.
		* Then select the closest edge out of them by comparing true line to point distances.
		* Return the closest edge info and calculate closest point on the edge from point.
		*/
	string closest_edge_sql = "WITH closest_candidates AS ( " \
									"SELECT e.uid, e.geog, e.from_node, e.to_node, e.length " \
									"FROM " + table_name + " as e " \
									"ORDER BY e.geog <-> " + point + " " \
									"LIMIT 100 " \
								"), " \
								"closest_edge as ( " \
									"SELECT uid, from_node, to_node, length, geog " \
									"FROM closest_candidates " \
									"ORDER BY ST_Distance(geog, " + point + ") " \
									"LIMIT 1 " \
								") " \
								+ db_graph->GetEdgeSelect() + 
								"ST_AsText(ST_Transform( " \
									"ST_ClosestPoint( " \
										"ST_Transform(geog::geometry, 3785), " \
										"ST_Transform(" + point + "::geometry, 3785) " \
									"), " \
									"4326)) " \
								"FROM closest_edge "; \
	pqxx::nontransaction n{connection_};
	pqxx::result result{n.exec(closest_edge_sql)};
	return db_graph->GetEdgeIterator(result.begin(), result.end());
}


std::string DatabaseHelper::MakeSTPoint(utility::Point p) {
	return "POINT(" + to_string(p.lon_) + " " + to_string(p.lat_) + ")";
}


std::string DatabaseHelper::MakeGeographyPoint(utility::Point p) {
	return "'SRID=4326;" + MakeSTPoint(p) + "'::geography";
}

std::string DatabaseHelper::CalculateRadius(utility::Point start, utility::Point end, double mult)  {
	return " 2000 + " + std::to_string(mult) + " * ST_Distance(" + MakeGeographyPoint(start) + ", " \
			"" + MakeGeographyPoint(end) + ") ";
}

/**
 * Example of such sql:
WITH closest_candidates AS (
SELECT e.osm_id, e.uid, e.geog, e.from_node, e.to_node, e.length
FROM czedges as e
WHERE shortcut = false
ORDER BY
e.geog <-> 'SRID=4326;POINT(13.394182 49.725673)'::geography
LIMIT 100
),
closest_edge AS (SELECT *
FROM closest_candidates
ORDER BY
ST_Distance(geog, 'SRID=4326;POINT(13.394182 49.725673)'::geography)
LIMIT 1
),
blade_point AS (
SELECT --*, 'SRID=4326;POINT(13.394182 49.725673)'::geography, st_asText(geog),
	(st_transform(
		st_closestpoint(
			st_transform(geog::geometry, 3785),
			st_transform('SRID=4326;POINT(13.394182 49.725673)'::geometry, 3785)
		),
		4326)) AS geog
FROM closest_edge
),
segments as (
SELECT st_transform(
	(
		st_dump(ST_Split(
			ST_Snap(l.geog::geometry, blade.geog::geometry, 0.0000001),
			blade.geog::geometry
		))
	).geom
	, 4326)::geography AS geog
FROM closest_edge AS l,
	blade_point AS blade
	),
max_uid AS (SELECT MAX(uid) AS uid FROM czedges),
adjacent AS (
SELECT czedges.from_node, czedges.to_node, ST_Length(segments.geog) as seg_len
FROM czedges INNER JOIN segments ON (ST_Intersects(segments.geog, czedges.geog)),
closest_edge AS ce
WHERE not ((ce.from_node = czedges.from_node and ce.to_node = czedges.to_node) or
	(ce.from_node = czedges.to_node and ce.to_node = czedges.from_node))
LIMIT 1
)
SELECT adjacent.from_node, adjacent.to_node, e.from_node, e.to_node, ST_Length(segments.geog), (segments.geog), adjacent.seg_len, max_uid.uid
FROM segments, closest_edge as e, max_uid,  adjacent
	*/
vector<DbRow> DatabaseHelper::GetClosestSegments(utility::Point p, const std::string &table_name, DbGraph* db_graph) {
	string point = MakeGeographyPoint(p);
	/*
		* First compute closest edge to `point` by fast finding (compare bounding rectangles)
		* the 100 candidates and selecting the best from them.
		*
		* Then calculate the closest point on the found edge to the `point`.
		*
		* Split the edge using the point to two segments (Snap closest point
		* to line - then it can be split by it 100%).
		*
		* Get max edge uid used in graph.
		*
		* Select ad edge that is adjacent to one of the segments (they touch at one intersection)
		* and calculate distance of the segment to later identify which one it was.
		*
		* Put it all together and return endpoints of adjacent edge, closest edge,
		* segment length, segment geoJSON geometry, previous query segment length and max uid.
		*/
	string sql = "WITH closest_candidates AS ( " \
					"  SELECT * " \
					"  FROM " + table_name + " as e " \
					"  WHERE " + db_graph->GetEndpointEdgeCondition("e") + " " \
					"  ORDER BY " \
					"    e.geog <-> " + point + " " \
					"  LIMIT 100 " \
					"), " \
					"closest_edge AS (SELECT * " \
					"FROM closest_candidates " \
					"ORDER BY " \
					"  ST_Distance(geog, " + point + ") " \
					"LIMIT 1 " \
					"), " \
					"blade_point AS ( " \
					"     SELECT " \
					"          (st_transform( " \
					"               st_closestpoint( " \
					"                    st_transform(geog::geometry, 3785), " \
					"                    st_transform(" + point + "::geometry, 3785) " \
					"               ), " \
					"               4326)) AS geog " \
					"     FROM closest_edge " \
					"), " \
					" segments as ( " \
					"  SELECT st_transform( " \
					"       ( " \
					"           st_dump(ST_Split( " \
					"              ST_Snap(l.geog::geometry, blade.geog::geometry, 0.0000001), " \
					"              blade.geog::geometry " \
					"          )) " \
					"         ).geom " \
					"       , 4326)::geography AS geog " \
					"  FROM closest_edge AS l, " \
					"       blade_point AS blade " \
					"       ), " \
					"max_uid AS (SELECT MAX(uid) AS uid FROM " + table_name + "), " \
					"adjacent AS ( " \
					"SELECT e.from_node, e.to_node, ST_Length(segments.geog) as seg_len " \
					"FROM " + table_name + " as e INNER JOIN segments ON (ST_Intersects(segments.geog, e.geog)), " \
					"     closest_edge AS ce " \
					"WHERE not ((ce.from_node = e.from_node and ce.to_node = e.to_node) or " \
					"       (ce.from_node = e.to_node and ce.to_node = e.from_node)) " \
					"LIMIT 1 " \
					") " \
					"SELECT adjacent.from_node, adjacent.to_node, e.from_node, e.to_node, ST_Length(segments.geog), ST_AsGeoJSON(segments.geog), adjacent.seg_len, max_uid.uid  " \
					"  FROM segments, closest_edge as e, max_uid,  adjacent " ;

	pqxx::nontransaction n{connection_};
	pqxx::result result{n.exec(sql)};

	std::vector<DbRow> result_rows{};
	for (pqxx::result::const_iterator c = result.begin(); c != result.end(); ++c) {
		result_rows.push_back(DbRow{c});
	}
	return result_rows;
}

bool DatabaseHelper::AddShortcutColumns(const std::string& table_name) {
	try {
	std::string sql = "ALTER TABLE " + table_name + " ADD COLUMN IF NOT EXISTS shortcut boolean; " \
						"ALTER TABLE " + table_name + " ADD COLUMN IF NOT EXISTS contracted_vertex bigint; " \
						"UPDATE " + table_name + " set shortcut = false; " \
						"UPDATE " + table_name + " set contracted_vertex = 0; ";
	pqxx::work w(connection_);
	w.exec(sql);
	w.commit();
	} catch (const std::exception& e) {
		return false; 
	}
	return true;
	
}

uint64_t DatabaseHelper::GetMaxEdgeId(const std::string& table_name) {
	std::string sql = "select max(uid) from " + table_name + ";";
	pqxx::nontransaction n{connection_};
	pqxx::result result{n.exec(sql)};
	return (result.begin())[0].as<uint64_t>();
}

void DatabaseHelper::DropGeographyIndex(const std::string& table_name) {
	std::string sql = "DROP INDEX IF EXISTS " + GetGeographyIndexName(table_name);
	pqxx::work w(connection_);
	w.exec(sql);
	w.commit();
}

void DatabaseHelper::CreateGeographyIndex(const std::string& table_name) {
	std::string sql = "CREATE INDEX IF NOT EXISTS " + GetGeographyIndexName(table_name) + " ON " + table_name + " USING GIST (geog);";
	pqxx::work w(connection_);
	w.exec(sql);
	w.commit();
}

void DatabaseHelper::CreateGraphTable(const std::string& graph_table_name, const std::string& new_table_name, DbGraph* db_graph) {
	std::string drop_table_sql = "DROP TABLE IF EXISTS " + new_table_name + "; ";
					
	std::string create_table_sql = db_graph->GetCreateGraphTable(graph_table_name, new_table_name);
	std::string sql = drop_table_sql + create_table_sql;
	pqxx::work w(connection_);
	w.exec(sql);
	w.commit();
}

void DatabaseHelper::CreateGreenIndex(const std::string& edges_table, const std::string& osm_polygons_table, const std::string& green_index_table) {
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
			"CREATE INDEX " + green_places_table_index + " on " + green_places_table + " USING GIST (way); "
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
	pqxx::work w(connection_);
    w.exec(sql);
    w.commit();
}

std::string DatabaseHelper::GetGeographyIndexName(const std::string& table_name) {
	return table_name + "_gix";
}



}