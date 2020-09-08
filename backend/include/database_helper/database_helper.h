#ifndef BACKEND_DATABASEHELPER_H
#define BACKEND_DATABASEHELPER_H
#include <string>
#include <string_view>
#include <pqxx/pqxx>
#include <iostream>
#include <unordered_map>
#include <vector>

class IDatabaseHelper {

};

class DbRow{

};

class EdgeDbRow {
    pqxx::result::const_iterator c_;
public:
    EdgeDbRow(const pqxx::result::const_iterator & c) : c_(c) {}

    template <typename T>
    T get(int i) {
        return c_[i].as<T>();
    }

};

class DatabaseHelper {
private:
    std::string db_name_;
    std::string user_;
    std::string password_;
    std::string host_address_;
    std::string port_;

    pqxx::connection connection_;

public:
    DatabaseHelper(const std::string & db_name, const std::string & user, const std::string & password,
                   const std::string & host_address, const std::string & port);

    ~DatabaseHelper();

    bool IsDbOpen();
    void CreateGraphTable(const std::string & sql);

    void SearchFor(const std::string & s);

    EdgeDbRow FindClosestEdge(double lon, double lat, const std::string & table_name);

    std::string MakeSTPoint(double lon, double lat);

//    template <typename Edge>
//    std::string GetRouteCoordinates(std::vector<Edge> edges) {
//
//        std::string sql_select = "select "
//    }

    template <typename Graph>
    void LoadGraph(double lon, double lat, double radius, const std::string & table_name, Graph & graph) {

        std::string center = MakeSTPoint(lon, lat);
        std::string load_graph_sql = "select uid, from_node, to_node, length " \
                            "from " + table_name + " as e " \
                            "where ST_DWithin('SRID=4326;" + center + "'::geography, e.geog, " + std::to_string(radius) + ") ";
        pqxx::nontransaction n{connection_};
        pqxx::result result{n.exec(load_graph_sql)};

        for (pqxx::result::const_iterator c = result.begin(); c != result.end(); ++c) {
            EdgeDbRow row{c};
            graph.AddEdge(row);
        }
    }



};

/*
-- "Closest" 100 streets to Broad Street station are?long 13.391480 lat 49.726250   49.7262000N, 13.3915000E
WITH closest_candidates AS (
  SELECT e.osm_id, e.uid, e.geog, e.from_node, e.to_node, e.length
  FROM cz_edges as e
  ORDER BY
    e.geog <-> 'SRID=4326;POINT(13.3915000 49.7262000)'::geography
  LIMIT 100
)
SELECT uid, from_node, to_node, length
FROM closest_candidates
ORDER BY
  ST_Distance(geog, 'SRID=4326;POINT(13.3915000 49.7262000)'::geography)
LIMIT 1;
 */

/*
select uid, from_node, to_node, length from cz_edges as e where ST_DWithin('SRID=4326;POINT(13.3915000 49.7262000)'::geography, e.geog, 2000);
*/
#endif //BACKEND_DATABASEHELPER_H