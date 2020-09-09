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

namespace database {

    class EdgeDbRow {
        pqxx::result::const_iterator c_;
    public:
        EdgeDbRow(const pqxx::result::const_iterator & c) : c_(c) {}

        template <typename T>
        T get(int i) {
            return c_[i].as<T>();
        }

    };

    struct Point {
        double lon_;
        double lat_;

        Point(double lon, double lat);

        Point(const Point & other);

        Point(Point && other);

        Point& operator=(const Point & other);

        Point& operator=(Point && other);

        ~Point();

        void Swap(Point & other);
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

        EdgeDbRow FindClosestEdge(Point, const std::string & table_name);

        std::string MakeSTPoint(Point);

        std::string MakeGeographyPoint(Point);

        std::string CalculateRadius(Point start, Point end, double mult);

        template <typename Edge>
        std::string GetRouteCoordinates(std::vector<Edge> edges, const std::string & table_name) {

            std::string sql_start = " SELECT ST_AsGeoJSON(geog) " \
                                " FROM " + table_name + " WHERE ";
            auto fold = [](std::string a, Edge e) {
                return std::move(a) + " or uid = " + std::to_string(e.get_uid());
            };
            std::string first_cond = " uid = " + std::to_string(edges[0].get_uid()) + " ";
            std::string cond = std::accumulate(std::next(edges.begin()), edges.end(), first_cond, fold);
            std::string sql_end = " ;";
            std::string complete_sql = sql_start + cond + sql_end;

            pqxx::nontransaction n(connection_);
            pqxx::result result{n.exec(complete_sql)};

            auto comma_fold = [](std::string a, pqxx::result::const_iterator it) {
                return std::move(a) + ',' + it[0].as<std::string>();
            };
            std::string geojson_list_start = "[" + (result.begin())[0].as<std::string>();
            std::string geojson_list_end = "]";
            std::string geojson_list_result = std::accumulate(std::next(result.begin()), result.end(), geojson_list_start, comma_fold) + geojson_list_end;
            return geojson_list_result;
        }

        template <typename Graph>
        void LoadGraph(Point center, std::string radius, const std::string & table_name, Graph & graph) {
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

 select *
from cz_edges as e where ST_DWithin('SRID=4326;POINT(13.393348750000001 49.723055299999999)'::geography,
									e.geog,
									0.7 * ST_Distance('SRID=4326;POINT(13.3998825 49.7230553)'::geography,
										'SRID=4326;POINT(13.3868150 49.7282850)'::geography));
*/
}
#endif //BACKEND_DATABASEHELPER_H