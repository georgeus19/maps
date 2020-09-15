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

    class EdgeDbRow {
        pqxx::result::const_iterator c_;
    public:
        EdgeDbRow(const pqxx::result::const_iterator & c) : c_(c) {}

        template <typename T>
        T get(int i) {
            return c_[i].as<T>();
        }

    };

    class DbRow {
        pqxx::result::const_iterator c_;
    public:
        DbRow(const pqxx::result::const_iterator & c) : c_(c) {}

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

        EdgeDbRow FindClosestEdge(utility::Point, const std::string & table_name);

        std::string MakeSTPoint(utility::Point);

        std::string MakeGeographyPoint(utility::Point);

        std::string CalculateRadius(utility::Point start, utility::Point end, double mult);

        std::vector<DbRow> GetClosestSegments(utility::Point, const std::string & table_name);

        template <typename Edge>
        std::vector<DbRow> CalculateEdgeSegments(const std::string & edge_geography, const std::string & blade_geography,
                                                 const Edge & next_edge, const std::string & table_name);

        template <typename Edge>
        std::string GetRouteCoordinates(std::vector<Edge> & edges, const std::string & table_name, const Edge & start_closest_edge, EdgeDbRow r_start,
                                                        const Edge & end_closest_edge, EdgeDbRow r_end);

        template <typename Edge>
        std::string GetRouteCoordinates(std::vector<Edge> & edges, const std::string & table_name);

        template <typename Graph>
        void LoadGraph(utility::Point center, std::string radius, const std::string & table_name, Graph & graph);

    private:

        std::pair<double, std::string> GetSegmentDistanceToStart(DbRow r);


        template <typename Edge>
        std::string SelectEndPointSegment(const Edge & first_edge, const Edge & second_edge, const std::string & table_name,
                                                          const Edge & closest_edge, EdgeDbRow r, bool & skip_edge);
        
    };


    template <typename Edge>
    std::string DatabaseHelper::SelectEndPointSegment(const Edge & first_edge, const Edge & second_edge, const std::string & table_name,
                                                      const Edge & closest_edge, EdgeDbRow r, bool & skip_edge) {
        Edge next_edge;

        bool first_edge_is_closest = first_edge == closest_edge;
        if (first_edge_is_closest) {
            next_edge = second_edge;
        } else {
            next_edge = first_edge;
        }
        std::string edge_geography = r.get<std::string>(4);
        std::string closest_point = r.get<std::string>(5);

        std::vector<DbRow> start_segment_rows = CalculateEdgeSegments(edge_geography, closest_point, next_edge, table_name);

        std::pair<double, std::string> correct_segment = std::make_pair(std::numeric_limits<double>::max(), "");
        for(auto&& r : start_segment_rows) {
            std::pair<double, std::string> segment = GetSegmentDistanceToStart(r);
            if (segment.first < correct_segment.first) {
                correct_segment = std::move(segment);
            }
        }
        
        skip_edge = first_edge_is_closest;
        return correct_segment.second;
    }

    template <typename Edge>
    std::string DatabaseHelper::GetRouteCoordinates(std::vector<Edge> & edges, const std::string & table_name) {

        std::string sql_start = " SELECT ST_AsGeoJSON(geog) " \
                                " FROM " + table_name + " WHERE ";
        auto fold = [](std::string a, Edge e) {
            return std::move(a) + " or uid = " + std::to_string(e.get_uid());
        };

        std::string first_cond = " uid = " + std::to_string((edges.begin())->get_uid());
        std::string cond = std::accumulate(edges.begin(), edges.end(), first_cond, fold);
        std::string sql_end = " ;";
        std::string complete_sql = sql_start + cond + sql_end;

        pqxx::nontransaction n(connection_);
        pqxx::result result{n.exec(complete_sql)};

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
    FROM cz_edges
    WHERE uid = 3
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

        // Split the edge with the point (blade). It is important to snap line to blade so that
        // line always contains blade when split is executed.
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