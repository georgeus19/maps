//
// Created by hrubyk on 10.08.20.
//

#include "database/database_helper.h"

using namespace std;
namespace database {

    Point::Point(double lon, double lat) : lon_(lon), lat_(lat) {}

    Point::Point(const Point & other) {
        lon_ = other.lon_;
        lat_ = other.lat_;
    }

    Point::Point(Point && other) {
        lon_ = other.lon_;
        lat_ = other.lat_;
    }

    Point& Point::operator=(const Point & other) {
        Point tmp{other};
        Swap(tmp);
        return *this;
    }

    Point& Point::operator=(Point && other) {
        if (this != &other) {
            lon_ = other.lon_;
            lat_ = other.lat_;
        }
        return *this;
    }

    Point::~Point() {}

    void Point::Swap(Point & other) {
        std::swap(lon_, other.lon_);
        std::swap(lat_, other.lat_);
    }

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

    EdgeDbRow DatabaseHelper::FindClosestEdge(Point p, const string & table_name){
        string point = MakeSTPoint(p);
        // "Closest" 100 streets to Broad Street station are?long 13.391480 lat 49.726250   49.7262000N, 13.3915000E
        string closest_edge_sql = "WITH closest_candidates AS ( " \
                                  "SELECT e.uid, e.geog, e.from_node, e.to_node, e.length " \
                                  "FROM " + table_name + " as e " \
                                  "ORDER BY e.geog <-> 'SRID=4326;" + point + "'::geography " \
                                  "LIMIT 100 " \
                              ") " \
                              "SELECT uid, from_node, to_node, length " \
                              "FROM closest_candidates " \
                              "ORDER BY ST_Distance(geog, 'SRID=4326;" + point + "'::geography) " \
                              "LIMIT 1; ";
        pqxx::nontransaction n{connection_};
        pqxx::result result{n.exec(closest_edge_sql)};

        pqxx::result::const_iterator c = result.begin();
        return EdgeDbRow{c};
        // do sth with the result...
    }


    std::string DatabaseHelper::MakeSTPoint(Point p) {
        return "POINT(" + to_string(p.lon_) + " " + to_string(p.lat_) + ")"; // e.g. POINT(13.3915000 49.7262000)
    }


    std::string DatabaseHelper::MakeGeographyPoint(Point p) {
        return "'SRID=4326;" + MakeSTPoint(p) + "'::geography";
    }

    std::string DatabaseHelper::CalculateRadius(Point start, Point end, double mult)  {
        return std::to_string(mult) + " * ST_Distance(" + MakeGeographyPoint(start) + ", " \
                "" + MakeGeographyPoint(end) + ") ";
    }

}
