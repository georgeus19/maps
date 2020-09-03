//
// Created by hrubyk on 10.08.20.
//

#include "database_helper.h"
using namespace std;

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

void DatabaseHelper::FindClosestEdge(double lon, double lat, const string & table_name){
    string point = MakeSTPoint(lon, lat);
    // "Closest" 100 streets to Broad Street station are?long 13.391480 lat 49.726250   49.7262000N, 13.3915000E
    string closest_edge_sql = "WITH closest_candidates AS ( " \
                                  "SELECT e.osm_id, e.geog " \
                                  "FROM " + table_name + " as e " \
                                  "ORDER BY e.geog <-> 'SRID=4326;" + point + "'::geography " \
                                  "LIMIT 100 " \
                              ") " \
                              "SELECT osm_id, geog " \
                              "FROM closest_candidates " \
                              "ORDER BY ST_Distance(geog, 'SRID=4326;" + point + "'::geography) " \
                              "LIMIT 1; ";
    pqxx::nontransaction n{connection_};
    pqxx::result result{n.exec(closest_edge_sql)};

    // do sth with the result...
}

void DatabaseHelper::LoadGraph(double lon, double lat, double radius, const string & table_name) {
    string center = MakeSTPoint(lon, lat);
    string load_graph_sql = "select * " \
                            "from " + table_name + " as e " \
                            "where ST_DWithin('SRID=4326;" + center + "'::geography, e.geog, " + to_string(radius) + ") ";
    pqxx::nontransaction n{connection_};
    pqxx::result result{n.exec(load_graph_sql)};

    // do sth with the result...
}

std::string DatabaseHelper::MakeSTPoint(double lon, double lat) {
    return "POINT(" + to_string(lon) + " " + to_string(lat) + ")"; // e.g. POINT(13.3915000 49.7262000)
}

