#include "database/database_helper.h"

using namespace std;
namespace database {

    const string kHouseNameTag = "\"addr:housename\"";
    const string kHouseNumberTag = "\"addr:housenumber\"";

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

    void DatabaseHelper::CreateGraphTable(const string & sql) {
        pqxx::work w(connection_);
        w.exec(sql);
        w.commit();
    }

    void DatabaseHelper::SearchFor(const string & s) {
        string sql = "SELECT " + kHouseNameTag + " " \
        "FROM planet_osm_polygon " \
        "WHERE " + kHouseNameTag + " like '" + s + "%' " \
        "LIMIT 5";
        pqxx::nontransaction n(connection_);

        pqxx::result query_result(n.exec(sql));

        for (pqxx::result::const_iterator it = query_result.cbegin(); it != query_result.end(); ++it) {
            std::cout << it[0].as<string>() << std::endl;
        }
    }

    EdgeDbRow DatabaseHelper::FindClosestEdge(utility::Point p, const string & table_name){
        string point = MakeGeographyPoint(p);
        // "Closest" 100 streets to Broad Street station are?long 13.391480 lat 49.726250   49.7262000N, 13.3915000E
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
                                   ")" \
                                  "SELECT uid, from_node, to_node, length, st_AsText(geog), " \
                                  "ST_AsText(ST_Transform( " \
                                      "ST_ClosestPoint( " \
                                          "ST_Transform(geog::geometry, 3785), " \
                                          "ST_Transform(" + point + "::geometry, 3785) " \
                                      "), " \
                                      "4326)) " \
                                  "FROM closest_edge "; \
        pqxx::nontransaction n{connection_};
        pqxx::result result{n.exec(closest_edge_sql)};

        pqxx::result::const_iterator c = result.begin();
        return EdgeDbRow{c};
    }


    std::string DatabaseHelper::MakeSTPoint(utility::Point p) {
        return "POINT(" + to_string(p.lon_) + " " + to_string(p.lat_) + ")"; // e.g. POINT(13.3915000 49.7262000)
    }


    std::string DatabaseHelper::MakeGeographyPoint(utility::Point p) {
        return "'SRID=4326;" + MakeSTPoint(p) + "'::geography";
    }

    std::string DatabaseHelper::CalculateRadius(utility::Point start, utility::Point end, double mult)  {
        return " 2000 + " + std::to_string(mult) + " * ST_Distance(" + MakeGeographyPoint(start) + ", " \
                "" + MakeGeographyPoint(end) + ") ";
    }

    std::pair<double, std::string> DatabaseHelper::GetSegmentDistanceToStart(DbRow r) {
        double min = std::numeric_limits<double>::max();
        size_t segment_index = 4;
        for (size_t i = 0; i < 4; ++i) {
            double dist = r.get<double>(i);
            if (dist < min) {
                min = dist;
            }
        }
        return std::make_pair(min, r.get<std::string>(segment_index));
    }

    vector<DbRow> DatabaseHelper::GetClosestSegments(utility::Point p, const std::string &table_name) {
        string point = MakeGeographyPoint(p);
        string sql = "WITH closest_candidates AS ( " \
                     "  SELECT e.osm_id, e.uid, e.geog, e.from_node, e.to_node, e.length " \
                     "  FROM " + table_name + " as e " \
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

}
