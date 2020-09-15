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
    std::string DatabaseHelper::GetRouteCoordinates(std::vector<Edge> & edges, const std::string & table_name, const Edge & start_closest_edge, EdgeDbRow r_start,
                                                    const Edge & end_closest_edge, EdgeDbRow r_end) {
        bool max_two_edges = false;
        Edge first_edge;
        Edge second_edge;
        Edge last_edge;
        Edge second_last_edge;

            // 2 adjacent streets(len 2), 1 same street(len 1), 1 street in middle(len 3)
        if (edges.size() == 1) {
            // 1 street in middle(len 3). PEIEIEP
            if (edges[0] != start_closest_edge && edges[0] != end_closest_edge) {
//                edges.push_back(edges[0]);
                first_edge = edges[0];
                second_edge = edges[0];
                last_edge = edges[0];
                second_last_edge = edges[0];
            } else { // PEIEP
                first_edge = start_closest_edge; // WHY?
                second_edge = start_closest_edge;
                last_edge = end_closest_edge;
                second_last_edge = end_closest_edge;
                max_two_edges = true;
            }
        } else {
            first_edge = edges[0];
            second_edge = edges[1];
            last_edge = edges[edges.size() - 1];
            second_last_edge = edges[edges.size() - 2];
        }
        bool skip_last_edge = false; // edges are reversed - first edge is the one around end of route!!
        std::string first_segment = std::move(SelectEndPointSegment<Edge>(last_edge, second_last_edge, table_name, start_closest_edge, r_start, skip_last_edge));

        bool skip_first_edge = false;
        std::string last_segment = std::move(SelectEndPointSegment<Edge>(first_edge, second_edge, table_name, end_closest_edge, r_end, skip_first_edge));

        std::string sql_start = " SELECT ST_AsGeoJSON(geog) " \
                                " FROM " + table_name + " WHERE ";
        auto fold = [](std::string a, Edge e) {
            return std::move(a) + " or uid = " + std::to_string(e.get_uid());
        };
        
        auto&& from_it = skip_first_edge ? std::next(edges.begin()) : edges.begin();
        auto&& to_it = skip_last_edge ? std::prev(edges.end()) : edges.end();

        if (from_it == to_it || max_two_edges) {
            return '[' + first_segment + ',' + last_segment + ']';
        }

        std::string first_cond = " uid = " + std::to_string(from_it->get_uid());
        std::string cond = std::accumulate(from_it, to_it, first_cond, fold);
        std::string sql_end = " ;";
        std::string complete_sql = sql_start + cond + sql_end;

        pqxx::nontransaction n(connection_);
        pqxx::result result{n.exec(complete_sql)};

        auto comma_fold = [](std::string a, pqxx::result::const_iterator it) {
            return std::move(a) + ',' + it[0].as<std::string>();
        };
        std::string geojson_list_start = '[' + first_segment;
        std::string geojson_list_end = ',' + last_segment + ']';
        std::string geojson_list_result = std::accumulate(result.begin(), result.end(), geojson_list_start, comma_fold) + geojson_list_end;
        return geojson_list_result;
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

/*
-- "Closest" 100 streets to Broad Street station are?long 13.391480 lat 49.726250   49.7262000N, 13.3915000E
WITH closest_candidates AS (
  SELECT e.osm_id, e.uid, e.geog, e.from_node, e.to_node, e.length
  FROM cz_edges as e
  ORDER BY
    e.geog <-> 'SRID=4326;POINT(13.3915000 49.7262000)'::geography
  LIMIT 100
),
closest_edge AS (SELECT *
FROM closest_candidates
ORDER BY
  ST_Distance(geog, 'SRID=4326;POINT(13.3915000 49.7262000)'::geography)
LIMIT 1
)
SELECT *, 'SRID=4326;POINT(13.3915000 49.7262000)'::geography, st_asText(geog),
st_astext(st_transform(
	st_closestpoint(
		st_transform(geog::geometry, 3785),
		st_transform('SRID=4326;POINT(13.3915000 49.7262000)'::geometry, 3785)
	),
	4326))
FROM closest_edge
 */

/*
select uid, from_node, to_node, length from cz_edges as e where ST_DWithin('SRID=4326;POINT(13.3915000 49.7262000)'::geography, e.geog, 2000);

 select *
from cz_edges as e where ST_DWithin('SRID=4326;POINT(13.393348750000001 49.723055299999999)'::geography,
									e.geog,
									0.7 * ST_Distance('SRID=4326;POINT(13.3998825 49.7230553)'::geography,
										'SRID=4326;POINT(13.3868150 49.7282850)'::geography));
*/
/*
 * TODO: Add edges representing segments to local graph when looking for the closest edges.
WITH closest_candidates AS (
  SELECT e.osm_id, e.uid, e.geog, e.from_node, e.to_node, e.length
  FROM czedges as e
  ORDER BY
    e.geog <-> 'SRID=4326;POINT(13.3915000 49.7262000)'::geography
  LIMIT 100
),
closest_edge AS (SELECT *
FROM closest_candidates
ORDER BY
  ST_Distance(geog, 'SRID=4326;POINT(13.3915000 49.7262000)'::geography)
LIMIT 1
),
blade_point AS (
	SELECT --*, 'SRID=4326;POINT(13.3915000 49.7262000)'::geography, st_asText(geog),
		(st_transform(
			st_closestpoint(
				st_transform(geog::geometry, 3785),
				st_transform('SRID=4326;POINT(13.3915000 49.7262000)'::geometry, 3785)
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
SELECT adjacent.from_node, adjacent.to_node, e.from_node, e.to_node, ST_Length(segments.geog), ST_AsGeoJSON(segments.geog), adjacent.seg_len, max_uid.uid
  FROM segments, closest_edge as e, max_uid,  adjacent
 */

}
#endif //BACKEND_DATABASEHELPER_H