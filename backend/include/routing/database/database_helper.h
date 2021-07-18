#ifndef ROUTING_DATABASE_DATABASE_HELPER_H
#define ROUTING_DATABASE_DATABASE_HELPER_H
#include "routing/database/csv_convertor.h"
#include "routing/database/db_graph.h"
#include "routing/database/db_edge_iterator.h"
#include "routing/types.h"

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
#include "routing/utility/point.h"
#include <filesystem>

namespace routing {
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
     * Find an edge that is closest to 'endpoint` and split it into segments.
     * The segments have one intersection at the location on the closest edge
     * the `endpoint` is closest to. The other intersection is always an
     * intersection of the closest edge.
     *
     * By having segments it is possible to calculate best paths much more
     * accurately since it is possible to start the routing algorithm
     * at the closest point in graph from the `endpoint`.
     *
     * @param table_name Table from which segments are got.
     * @param endpoint Endpoint of a route for which segments are found.
     * @return Vector of DbRows each representing one segment. Each segment can be transformed to graph edge.
     */
    std::vector<DbRow> GetClosestSegments(utility::Point endpoint, const std::string& table_name, DbGraph* db_graph);

    /**
     * Find geometries of all edges in `edges` and return them.
     *
     * @tparam Edge Graph Edge type.
     * @param edges Vector of edges whose geometries are found.
     * @param table_name Table where to look for geometries.
     * @return String of geometries in geoJSON format.
     */
    template <typename Edge>
    std::string GetRouteCoordinates(std::vector<Edge>::const_iterator begin, std::vector<Edge>::const_iterator end, const std::string& table_name);

    /**
     * Load the entire graph from the database table (edgelist)
     */
    template <typename Graph, typename EdgeFactory>
    void LoadGraphEdges(const std::string& table_name, Graph& graph, DbGraph* db_graph, EdgeFactory& edge_factory);

    /**
     * Appends shortcuts of the graph to the given table - needs to have appropriate columns.
     */ 
    template <typename Graph, typename EdgeConvertor>
    void SaveEdges(const std::string& table_name, const std::string& geom_table, Graph& graph, const EdgeConvertor& edge_convertor, DbGraph* db_graph);

    template <typename Graph, typename VertexConvertor>
    void SaveVertices(const std::string& table_name, Graph& graph, const VertexConvertor& vertex_convertor, DbGraph* db_graph);

    void DropGeographyIndex(const std::string& table_name);

    void CreateGeographyIndex(const std::string& table_name);

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
std::string DatabaseHelper::GetRouteCoordinates(std::vector<Edge>::const_iterator begin, std::vector<Edge>::const_iterator end, const std::string& table_name) {

    std::string sql_start = " SELECT uid, ST_AsGeoJSON(geog) " \
                            " FROM " + table_name + " WHERE ";
    // Define fold function for std::accumulate.
    auto fold = [](const std::string& a, const Edge& e) {
        return std::move(a) + " or uid = " + std::to_string(e.get_uid());
    };

    // WHERE clause looks like: WHERE uid=3 or uid=42 ...
    std::string first_cond = " uid = " + std::to_string(begin->get_uid());
    std::string cond = std::accumulate(begin, end, first_cond, fold);
    std::string sql_end = " ;";
    std::string complete_sql = sql_start + cond + sql_end;

    pqxx::nontransaction n(*connection_);
    pqxx::result result{n.exec(complete_sql)};

    std::unordered_map<unsigned_id_type, std::string> geometries{};
    for(auto it = result.begin(); it != result.end(); ++it) {
        geometries.emplace(it[0].as<unsigned_id_type>(), it[1].as<std::string>());
    }
    std::string geojson = "";
    for(auto it = begin; it != end; ++it) {
        auto&& edge = *it;
        auto geom_it = geometries.find(edge.get_uid());
        if (geom_it != geometries.end()) {
            geojson += std::move(geom_it->second);
            geojson += ",";
        } else {
            std::cout << edge.get_uid() << " not found in edge table but is part of routing result." << std::endl;
        }

    }
    return geojson;
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

template <typename Graph, typename EdgeConvertor>
void DatabaseHelper::SaveEdges(const std::string& table_name, const std::string& geom_table, Graph& graph, const EdgeConvertor& edge_convertor, DbGraph* db_graph) {
    auto&& current_dir = std::filesystem::current_path();
    std::string data_path{current_dir.string() + "/graph.csv"};

    std::string drop_table = "DROP TABLE IF EXISTS " + table_name + "; ";
					
	std::string create_table = db_graph->GetCreateGraphTable(table_name);

    std::string copy = "COPY " + table_name + " FROM '" + data_path + "' DELIMITER ';' CSV NULL 'null'; ";
    std::string copy_geometry =
                "UPDATE " + table_name + " as new_table " \
                "SET geog = geom_table.geog " \
                "FROM " + geom_table + " as geom_table " \
                "WHERE geom_table.uid = new_table.uid; ";

    CsvConvertor convertor{data_path};
    convertor.SaveEdges(graph, edge_convertor, [](const typename Graph::Edge& edge) {
        bool twoway_condition = true;
        bool forward_or_twoway = edge.IsTwoway() || edge.IsForward();
        if (edge.IsTwoway()) {
            twoway_condition = edge.get_from() < edge.get_to();
        }
        return forward_or_twoway && twoway_condition;
    });
    std::string sql = drop_table + create_table + copy + copy_geometry;
    pqxx::work w(*connection_);
    w.exec(sql);
    w.commit();
}

template <typename Graph, typename VertexConvertor>
void DatabaseHelper::SaveVertices(const std::string& table_name, Graph& graph, const VertexConvertor& vertex_convertor, DbGraph* db_graph) {
    auto&& current_dir = std::filesystem::current_path();
    std::string data_path{current_dir.string() + "/" + table_name + ".csv"};
    std::string drop_table = "DROP TABLE IF EXISTS " + table_name + "; ";
    std::string create_table = db_graph->GetCreateVertexTable(table_name);
    std::string copy = "COPY " + table_name + " FROM '" + data_path + "' DELIMITER ';' CSV; ";
    CsvConvertor convertor{data_path};
    convertor.SaveVertices(graph, vertex_convertor);
    std::string sql = drop_table + create_table + " " + copy;
    pqxx::work w(*connection_);
    w.exec(sql);
    w.commit();
}

template <typename Graph>
void DatabaseHelper::LoadAdditionalVertexProperties(const std::string& vertices_table, Graph& g) {
    std::string sql = " SELECT uid, ordering_rank FROM " + vertices_table;
    pqxx::nontransaction n{*connection_};
    pqxx::result result{n.exec(sql)};
    for (auto&& it = result.begin(); it != result.end(); ++it) {
        unsigned_id_type vertex_id = it[0].as<unsigned_id_type>();
        auto&& vertex = g.GetVertex(vertex_id);
        vertex.set_ordering_rank(it[1].as<unsigned_id_type>());
    }
}



}
}
#endif //ROUTING_DATABASE_DATABASE_HELPER_H