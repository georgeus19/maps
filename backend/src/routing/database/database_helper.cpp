#include "routing/database/database_helper.h"

using namespace std;
namespace routing {
namespace database {

DatabaseHelper::DatabaseHelper(const string & db_name, const string & user, const string & password,
	const string & host_address, const string & port) : db_name_(db_name),user_(user), password_(password),
		host_address_(host_address),port_(port), connection_(std::make_unique<pqxx::connection>("dbname = " + db_name_ + " user = " + user_ +
		" password = " + password_ + " hostaddr = " + host_address_ + " port = " + port_)) {}

DatabaseHelper::~DatabaseHelper() {
	if (connection_) {
		connection_->disconnect();
	}
}

bool DatabaseHelper::IsDbOpen() {
	return connection_->is_open();
}

void DatabaseHelper::DisconnectIfOpen() {
	if (IsDbOpen()) {
		connection_->disconnect();
		connection_.reset();
	}
}

void DatabaseHelper::RunTransactional(const std::string& sql) {
	pqxx::work w(*connection_);
	w.exec(sql);
	w.commit();
}

void DatabaseHelper::RunNontransactional(const std::string& sql, const std::function<void(const DbRow&)> f) {
	pqxx::nontransaction n{*connection_};
	pqxx::result result{n.exec(sql)};

	for (pqxx::result::const_iterator c = result.begin(); c != result.end(); ++c) {
		f(DbRow{c});
	}
}

std::string DatabaseHelper::MakeSTPoint(utility::Point p) {
	return "POINT(" + to_string(p.lon_) + " " + to_string(p.lat_) + ")";
}


std::string DatabaseHelper::MakeGeographyPoint(utility::Point p) {
	return "'SRID=4326;" + MakeSTPoint(p) + "'::geography";
}

/**
 * Example of such sql:
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
adjacent AS (
SELECT czedges.from_node, czedges.to_node, ST_Length(segments.geog) as seg_len, czedges.geog
FROM czedges INNER JOIN segments ON (ST_Intersects(segments.geog, czedges.geog)),
closest_edge AS ce
WHERE not ((ce.from_node = czedges.from_node and ce.to_node = czedges.to_node) or
	(ce.from_node = czedges.to_node and ce.to_node = czedges.from_node))
LIMIT 1
),
segments_final as (
SELECT segments.geog, ST_Intersects(adjacent.geog, segments.geog) as intersects_adjacent
FROM adjacent, segments
	)
SELECT adjacent.from_node, adjacent.to_node, e.from_node, e.to_node, (ST_Length(s.geog) / ST_Length(e.geog)), s.intersects_adjacent, s.geog
FROM segments_final as s, closest_edge as e, adjacent
	*/
vector<DbRow> DatabaseHelper::GetClosestSegments(utility::Point p, const std::string& table_name, DbGraph* db_graph) {
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
					"adjacent AS ( " \
					"SELECT e.from_node, e.to_node, ST_Length(segments.geog) as seg_len, e.geog " \
					"FROM " + table_name + " as e INNER JOIN segments ON (ST_Intersects(segments.geog, e.geog)), " \
					"     closest_edge AS ce " \
					"WHERE not ((ce.from_node = e.from_node and ce.to_node = e.to_node) or " \
					"       (ce.from_node = e.to_node and ce.to_node = e.from_node)) " \
					"LIMIT 1 " \
					"), " \
					"segments_final as ( " \
					"SELECT segments.geog, ST_Intersects(adjacent.geog, segments.geog) as intersects_adjacent " \
					"FROM adjacent, segments " \
					"	) " \
					"SELECT adjacent.from_node, adjacent.to_node, e.from_node, e.to_node, e.uid, (ST_Length(s.geog) / ST_Length(e.geog)), ST_AsGeoJSON(s.geog), s.intersects_adjacent " \
					"  FROM segments_final as s, closest_edge as e, adjacent" ;

	pqxx::nontransaction n{*connection_};
	pqxx::result result{n.exec(sql)};

	std::vector<DbRow> result_rows{};
	for (pqxx::result::const_iterator c = result.begin(); c != result.end(); ++c) {
		result_rows.push_back(DbRow{c});
	}
	return result_rows;
}

void DatabaseHelper::DropGeographyIndex(const std::string& table_name) {
	std::string sql = "DROP INDEX IF EXISTS " + GetGeographyIndexName(table_name);
	pqxx::work w(*connection_);
	w.exec(sql);
	w.commit();
}

void DatabaseHelper::CreateGeographyIndex(const std::string& table_name) {
	std::string sql = "CREATE INDEX IF NOT EXISTS " + GetGeographyIndexName(table_name) + " ON " + table_name + " USING GIST (geog);";
	pqxx::work w(*connection_);
	w.exec(sql);
	w.commit();
}

std::string DatabaseHelper::GetGeographyIndexName(const std::string& table_name) {
	return table_name + "_gix";
}



}
}