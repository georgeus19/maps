#ifndef BACKEND_DATABASEHELPER_H
#define BACKEND_DATABASEHELPER_H
#include <string>
#include <string_view>
#include <pqxx/pqxx>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <numeric>
#include <functional>
#include <utility>
#include "utility/point.h"

namespace database {

/**
 * EdgeDbRow provides an interface to read a row of data representing graph edge from database
 * that can be used to create instance of Edge class so that
 * Edge itself does not need to know the detail interface of
 * library pqxx.
 */
class EdgeDbRow {
    /**
     * Iterator that represents an output row from db.
     */
    pqxx::result::const_iterator c_;
public:
    EdgeDbRow(const pqxx::result::const_iterator & c) : c_(c) {}

    /**
     * Get data from specified column from db row.
     *
     * @tparam T Type of data to return.
     * @param i Index of column.
     * @return Data from column i cast to T type.
     */
    template <typename T>
    T get(int i) {
        return c_[i].as<T>();
    }

};

/**
 * DbRow provides an interface to read a row of data from database
 * so that other classes do not need to know the detail interface of
 * library pqxx.
 */
class DbRow {
    /**
     * Iterator that represents an output row from db.
     */
    pqxx::result::const_iterator c_;
public:
    DbRow(const pqxx::result::const_iterator & c) : c_(c) {}

    /**
     * Get data from specified column from db row.
     *
     * @tparam T Type of data to return.
     * @param i Index of column.
     * @return Data from column i cast to T type.
     */
    template <typename T>
    T get(int i) {
        return c_[i].as<T>();
    }
};

/**
 * Class that provides a way to connect to a database and read (or write) data
 * from it so that no other classes need to know of how to use library pqxx
 * to connect to db directly.
 */
class DatabaseHelper {
private:
    /**
     * Database name.
     */
    std::string db_name_;

    /**
     * User name that it is connected to db under.
     */
    std::string user_;

    /**
     * User password to db.
     */
    std::string password_;

    /**
     * Host address of the db server.
     */
    std::string host_address_;

    /**
     * Port of the database server.
     */
    std::string port_;

    /**
     * Connection to certain database. It is possible to
     * execute queries on the database through it (-> read/write data).
     */
    pqxx::connection connection_;

public:
    DatabaseHelper(const std::string & db_name, const std::string & user, const std::string & password,
                    const std::string & host_address, const std::string & port);

    ~DatabaseHelper();

    /**
     * Check if a database connection was established correctly.
     *
     * @return True if it is possible to connect to db.
     */
    bool IsDbOpen();

    /**
     * Find a graph edge that would be closest to `point`.
     *
     * @param table_name Name of table where to search for the edge.
     * @param point Location of point whose closest edge is found.
     * @return EdgeDbRow representing the closest Edge.
     */
    EdgeDbRow FindClosestEdge(utility::Point point, const std::string & table_name);

    /**
     * Convert `point` to string point in WKT format = POINT(X Y)
     *
     * @param point Point to convert.
     * @return String WKT representation of `point`.
     */
    std::string MakeSTPoint(utility::Point point);

    /**
     * Convert `point` to postgis geography type(SRID=4326).
     * @param point Point to convert.
     * @return Postgis string representation of `point`
     */
    std::string MakeGeographyPoint(utility::Point point);

    /**
     * Calculate how big the radius should be to load a graph that
     * would contain the best path from `start` to `end` from the
     * point exactly between them. Resp. write a how-to calculate
     * the radius in postgis using string.
     *
     * @param start Start point of route.
     * @param end End point of route.
     * @param mult How much of distance between `start` and `end` should be taken into account.
     * @return String postgis calculation of radius.
     */
    std::string CalculateRadius(utility::Point start, utility::Point end, double mult);

    /**
     * Find an edge that is closest to 'endpoint` and split it into segments.
     * The segments have one intersection at the location on the closest edge
     * the `endpoint` is closest to. The other intersection is always an
     * intersection of the closest edge.
     *
     * By having segments it is possible to calculate best paths much more
     * accurately since it is possible to start the routing algorithm
     * at the closest point in graph from the `endpoint`.
     *
     * Better that CalculateEdgeSegments since it provides a way to put segments
     * to routing graph.
     *
     * @param table_name Table from which segments are got.
     * @param endpoint Endpoint of a route for which segments are found.
     * @return Vector of DbRows each representing one segment. Each segment can be transformed to graph edge.
     */
    std::vector<DbRow> GetClosestSegments(utility::Point endpoint, const std::string & table_name);

    /**
     * Split an edge into segments by a blade and return
     * geometries of each segment. Also returns the
     * distances to each segment's endpoint to each
     * `next_edge`'s endpoint so that it can be calculated
     * which segment is closer to `next_edge` and use it's geometry.
     *
     * @tparam Edge Graph Edge type.
     * @param edge_geography Edge geography that is split to segments.
     * @param blade_geography Blade geography that splits `edge`.
     * @param next_edge It's endpoints are used to calculate distance to segments' endpoint (to determine which is closer).
     * @param table_name Name of table where edges are saved.
     * @return Vector of DbRow where each represents one segment and the calculated distances.
     */
    template <typename Edge>
    std::vector<DbRow> CalculateEdgeSegments(const std::string & edge_geography, const std::string & blade_geography,
                                                const Edge & next_edge, const std::string & table_name);


    /**
     * Find geometries of all edges in `edges` and return them.
     *
     * @tparam Edge Graph Edge type.
     * @param edges Vector of edges whose geometries are found.
     * @param table_name Table where to look for geometries.
     * @return String of geometries in geoJSON format.
     */
    template <typename Edge>
    std::string GetRouteCoordinates(std::vector<Edge> & edges, const std::string & table_name);

    /**
     * Add edges to `graph` that are within `radius` from `center`.
     * @tparam Graph Routing graph class type.
     * @param center Center of the area edges are loaded to graph from.
     * @param radius How close a edge must be from `center` to be loaded to graph.
     * @param table_name Name of table where to look for edges.
     * @param graph Graph the edges are loaded into.
     */
    template <typename Graph>
    void LoadGraph(utility::Point center, std::string radius, const std::string & table_name, Graph & graph);

private:
    
};

template <typename Edge>
std::string DatabaseHelper::GetRouteCoordinates(std::vector<Edge> & edges, const std::string & table_name) {

    std::string sql_start = " SELECT ST_AsGeoJSON(geog) " \
                            " FROM " + table_name + " WHERE ";
    // Define fold function for std::accumulate.
    auto fold = [](std::string a, Edge e) {
        return std::move(a) + " or uid = " + std::to_string(e.get_uid());
    };

    // WHERE clause looks like: WHERE uid=3 or uid=42 ...
    std::string first_cond = " uid = " + std::to_string((edges.begin())->get_uid());
    std::string cond = std::accumulate(edges.begin(), edges.end(), first_cond, fold);
    std::string sql_end = " ;";
    std::string complete_sql = sql_start + cond + sql_end;

    pqxx::nontransaction n(connection_);
    pqxx::result result{n.exec(complete_sql)};

    // Get all GeoJSON geometries in one string separated by comma.
    auto comma_fold = [](std::string a, pqxx::result::const_iterator it) {
        return std::move(a) + ',' + it[0].as<std::string>();
    };
    std::string geojson_list_start = "";
    std::string geojson_list_result = std::accumulate(result.begin(), result.end(), geojson_list_start, comma_fold);
    return geojson_list_result;
}

/**
 * Example of sql:
WITH next_edge AS (
SELECT *
FROM czedges
WHERE uid = 8
),
segments as (
SELECT (st_dump(
    st_transform(
    ST_Split(
        ST_Snap(l, blade, 0.0000001),
        blade
    ),
    4326))).geom as segment
FROM ST_Transform('SRID=4326;LINESTRING(13.391116 49.7264131,13.3912178 49.7259554)'::geometry, 3785) as l,
    ST_Transform('SRID=4326;POINT(13.3911702145311 49.7261693481709)'::geometry, 3785) as blade
    )
SELECT ST_Distance(
        ST_StartPoint(next_edge.geog::geometry)::geography,
        ST_EndPoint(segment::geometry)::geography
    ),
    ST_Distance(
        ST_StartPoint(next_edge.geog::geometry)::geography,
        ST_StartPoint(segment::geometry)::geography
    ),
    ST_Distance(
        ST_EndPoint(next_edge.geog::geometry)::geography,
        ST_StartPoint(segment::geometry)::geography

    ),
    ST_Distance(
        ST_EndPoint(next_edge.geog::geometry)::geography,
        ST_EndPoint(segment::geometry)::geography
    ),
    ST_AsGeoJSON(segment),
    ST_AsGeoJSON(next_edge.geog)
FROM segments, next_edge
*/
template <typename Edge>
std::vector<DbRow> DatabaseHelper::CalculateEdgeSegments(const std::string & edge_geography, const std::string & blade_geography,
                                            const Edge & next_edge, const std::string & table_name) {

    std::string line =  "SRID=4326;" + edge_geography;
    std::string blade = "SRID=4326;" + blade_geography;

    // Find `next_edge` in table.
    // Split the edge with the point (blade). It is important to snap line to blade so that
    // line always contains blade when split is executed.
    // Calculate distances to endpoints of `next_edge` from segments' endpoints.
    std::string sql = " " \
                    "WITH next_edge AS ( " \
                    "    SELECT * " \
                    "    FROM " + table_name + " " \
                    "    WHERE uid = " + std::to_string(next_edge.get_uid()) + " " \
                    "), "
                                                                            "segments as ( " \
                    "    SELECT (ST_Dump( " \
                    "        ST_Transform( " \
                    "        ST_Split( " \
                    "            ST_Snap(l, blade, 0.0000001), " \
                    "            blade " \
                    "        ), " \
                    "        4326))).geom as segment " \
                    "    FROM ST_Transform('" + line + "'::geometry, 3785) as l, " \
                    "         ST_Transform('" + blade + "'::geometry, 3785) as blade " \
                    ") " \
                    "SELECT ST_Distance( " \
                    "        ST_StartPoint(next_edge.geog::geometry)::geography, " \
                    "        ST_EndPoint(segment::geometry)::geography " \
                    "    ), " \
                    "    ST_Distance( " \
                    "        ST_StartPoint(next_edge.geog::geometry)::geography, " \
                    "        ST_StartPoint(segment::geometry)::geography " \
                    "    ), " \
                    "    ST_Distance( " \
                    "        ST_EndPoint(next_edge.geog::geometry)::geography, " \
                    "        ST_StartPoint(segment::geometry)::geography " \
                    "    ), " \
                    "    ST_Distance( " \
                    "        ST_EndPoint(next_edge.geog::geometry)::geography, " \
                    "        ST_EndPoint(segment::geometry)::geography " \
                    "    ), " \
                    "    ST_AsGeoJSON(segment), "
                    "    ST_AsGeoJSON(next_edge.geog) " \
                    "FROM segments, next_edge "; \
    pqxx::nontransaction n{connection_};
    pqxx::result result{n.exec(sql)};

    std::vector<DbRow> result_rows{};
    for (pqxx::result::const_iterator c = result.begin(); c != result.end(); ++c) {
        result_rows.push_back(DbRow{c});
    }
    return result_rows;
}

/*
    * Example of query
select *
from czedges as e where ST_DWithin('SRID=4326;POINT(13.393348750000001 49.723055299999999)'::geography,
                                e.geog,
                                0.7 * ST_Distance('SRID=4326;POINT(13.3998825 49.7230553)'::geography,
                                    'SRID=4326;POINT(13.3868150 49.7282850)'::geography));
    */
template <typename Graph>
void DatabaseHelper::LoadGraph(utility::Point center, std::string radius, const std::string & table_name, Graph & graph) {
    // Select all edges within the `radius` of center.
    std::string load_graph_sql = "select uid, from_node, to_node, length " \
                        "from " + table_name + " as e " \
                        "where ST_DWithin('SRID=4326;" + MakeSTPoint(center) + "'::geography, e.geog, " + radius + ") ";
    pqxx::nontransaction n{connection_};
    pqxx::result result{n.exec(load_graph_sql)};

    for (pqxx::result::const_iterator c = result.begin(); c != result.end(); ++c) {
        EdgeDbRow row{c};
        graph.AddEdge(row);
    }
}


}
#endif //BACKEND_DATABASEHELPER_H