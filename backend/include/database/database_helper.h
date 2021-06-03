#ifndef BACKEND_DATABASEHELPER_H
#define BACKEND_DATABASEHELPER_H
#include <string>
#include <string_view>
#include <pqxx/pqxx>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <numeric>
#include <fstream>
#include <exception>
#include <functional>
#include <memory>
#include <utility>
#include "utility/point.h"
#include <filesystem>
#include "database/csv_convertor.h"
#include "database/db_graph.h"
#include "database/db_edge_iterator.h"

namespace database {



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
    DbRow(const pqxx::result::const_iterator& c) : c_(c) {}

    /**
     * Get data from specified column from db row.
     *
     * @tparam T Type of data to return.
     * @param i Index of column.
     * @return Data from column i cast to T type.
     */
    template <typename T>
    T get(int i) const {
        return c_[i].as<T>();
    }
};

/**
 * Class that provides a way to connect to a database and read (or write) data
 * from it so that no other classes need to know of how to use library pqxx
 * to connect to db directly.
 */
class DatabaseHelper {

public:
    DatabaseHelper(const std::string& db_name, const std::string& user, const std::string& password,
                    const std::string& host_address, const std::string& port);


    DatabaseHelper(const DatabaseHelper& other) = delete;
    DatabaseHelper(DatabaseHelper&& other) = default;
    DatabaseHelper& operator=(const DatabaseHelper& other) = delete;
    DatabaseHelper& operator=(DatabaseHelper&& other) = default;
    ~DatabaseHelper();

    /**
     * Check if a database connection was established correctly.
     *
     * @return True if it is possible to connect to db.
     */
    bool IsDbOpen();

    void DisconnectIfOpen();

    void RunTransactional(const std::string& sql);

    void RunNontransactional(const std::string& sql, const std::function<void(const DbRow&)> f);

    /**
     * Find a graph edge that would be closest to `point`.
     *
     * @param table_name Name of table where to search for the edge.
     * @param point Location of point whose closest edge is found.
     * @return EdgeDbRow representing the closest Edge.
     */
    std::unique_ptr<DbEdgeIterator> FindClosestEdge(utility::Point point, const std::string & table_name, DbGraph* db_graph);

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
    std::vector<DbRow> GetClosestSegments(utility::Point endpoint, const std::string & table_name, DbGraph* db_graph);

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
     * Load the entire graph from the database table (edgelist)
     */
    template <typename Graph, typename EdgeFactory>
    void LoadGraphEdges(const std::string& table_name, Graph& graph, DbGraph* db_graph, EdgeFactory& edge_factory);

    /**
     * Add columns to edge list table which consider shortcut edges.
     */
    bool AddShortcutColumns(const std::string& table_name);

    /**
     * Appends shortcuts of the graph to the given table - needs to have appropriate columns.
     */ 
    template <typename Graph>
    void AddShortcuts(const std::string& table_name, Graph& graph);

    /**
     * Creates a table with vertex ordering of the graph. Indexed by vertex ids.
     */ 
    template <typename Graph>
    void AddVertexOrdering(const std::string& table_name, Graph& graph);

    uint64_t GetMaxEdgeId(const std::string& table_name);

    void DropGeographyIndex(const std::string& table_name);

    void CreateGeographyIndex(const std::string& table_name);

    void CreateGraphTable(const std::string& graph_table_name, const std::string& new_table_name, DbGraph* db_graph);

    template <typename Graph>
    void LoadAdditionalVertexProperties(const std::string& vertices_table, Graph&g);  

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
    std::unique_ptr<pqxx::connection> connection_;

    std::string GetGeographyIndexName(const std::string& table_name);

};

template <typename Edge>
std::string DatabaseHelper::GetRouteCoordinates(std::vector<Edge>& edges, const std::string & table_name) {

    std::string sql_start = " SELECT uid, ST_AsGeoJSON(geog) " \
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

    pqxx::nontransaction n(*connection_);
    pqxx::result result{n.exec(complete_sql)};

    std::unordered_map<uint64_t, std::string> geometries{};
    for(auto it = result.begin(); it != result.end(); ++it) {
        geometries.emplace(it[0].as<uint64_t>(), it[1].as<std::string>());
    }
    std::string geojson = "";
    for(auto&& edge : edges) {
        auto&& nh = geometries.extract(edge.get_uid());
        if (!nh.empty()) {
            geojson += std::move(nh.mapped());
            geojson += ",";
        } else {
            std::cout << edge.get_uid() << " not found in czedges but is part of routing result." << std::endl;
        }
    }
    return geojson;
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
    pqxx::nontransaction n{*connection_};
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
template <typename Graph, typename EdgeFactory>
void DatabaseHelper::LoadGraphEdges(const std::string& table_name, Graph& graph, DbGraph* db_graph, EdgeFactory& edge_factory) {
    std::string sql = db_graph->GetEdgeSelect() + " FROM " + table_name + ";";
    
    pqxx::nontransaction n{*connection_};
    pqxx::result result{n.exec(sql)};
    
     for (std::unique_ptr<DbEdgeIterator> it = db_graph->GetEdgeIterator(result.begin(), result.end()); !(it->IsEnd()); it->Inc()) {
        graph.AddEdge(edge_factory.Create(*it));
    }
}


template <typename Graph>
void DatabaseHelper::AddShortcuts(const std::string& table_name, Graph& graph) {
    auto&& current_dir = std::filesystem::current_path();
    std::string data_path{current_dir.string() + "/shortcuts.csv"};
    std::string sql = "COPY " + table_name + " FROM '" + data_path + "' DELIMITER ';' CSV NULL 'null';";
    CsvConvertor convertor{data_path};
    convertor.SaveEdges(graph, [](const typename Graph::Edge& edge) {
        bool twoway_condition = true;
        bool forward_or_twoway = edge.IsTwoway() || edge.IsForward();
        if (edge.IsTwoway()) {
            twoway_condition = edge.get_from() < edge.get_to();
        }
        return forward_or_twoway && edge.IsShortcut() && twoway_condition;
    });

    pqxx::work w(*connection_);
    w.exec(sql);
    w.commit();
}

template <typename Graph>
void DatabaseHelper::AddVertexOrdering(const std::string& table_name, Graph& graph) {
    auto&& current_dir = std::filesystem::current_path();
    std::string data_path{current_dir.string() + "/" + table_name + ".csv"};
    std::string drop_table_sql = "DROP TABLE IF EXISTS " + table_name + "; ";
    std::string create_table_sql = "CREATE TABLE " + table_name + "("  \
        "osm_id BIGINT PRIMARY KEY, " \
        "ordering_rank BIGINT NOT NULL); ";
    std::string copy_sql = "COPY " + table_name + " FROM '" + data_path + "' DELIMITER ';' CSV; ";
    // string create_index = "CREATE INDEX " + table_name + "_osm_id_idx ON " + table_name + " (osm_id);";
    CsvConvertor convertor{data_path};
    convertor.SaveVertexOrdering(graph);
    std::string sql = drop_table_sql + create_table_sql + " " + copy_sql;
    pqxx::work w(*connection_);
    w.exec(sql);
    w.commit();
}

// template <typename Graph>
// void DatabaseHelper::LoadAdditionalVertexProperties(const std::string& vertices_table, Graph& g) {
//     std::string select_sql = " SELECT osm_id, ordering_rank FROM " + vertices_table;
//     std::string where_condition = " WHERE false = true ";
//     g.ForEachVertex([&](typename Graph::Vertex& v) {
//         where_condition += " or osm_id = ";
//         where_condition += std::to_string(v.get_osm_id());
//     });
//     where_condition += " ";
//     std::string sql = select_sql + where_condition;
//     pqxx::nontransaction n{*connection_};
//     pqxx::result result{n.exec(sql)};
//     for (auto&& it = result.begin(); it != result.end(); ++it) {
//         uint64_t vertex_id = it[0].as<uint64_t>();
//         auto&& vertex = g.GetVertex(vertex_id);
//         vertex.set_ordering_rank(it[1].as<uint64_t>());
//     }
// }

template <typename Graph>
void DatabaseHelper::LoadAdditionalVertexProperties(const std::string& vertices_table, Graph& g) {
    std::string sql = " SELECT osm_id, ordering_rank FROM " + vertices_table;
    pqxx::nontransaction n{*connection_};
    pqxx::result result{n.exec(sql)};
    for (auto&& it = result.begin(); it != result.end(); ++it) {
        uint64_t vertex_id = it[0].as<uint64_t>();
        auto&& vertex = g.GetVertex(vertex_id);
        vertex.set_ordering_rank(it[1].as<uint64_t>());
    }
}



}
#endif //BACKEND_DATABASEHELPER_H