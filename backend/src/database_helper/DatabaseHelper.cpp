//
// Created by hrubyk on 10.08.20.
//

#include "DatabaseHelper.h"
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