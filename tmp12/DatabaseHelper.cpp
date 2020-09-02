//
// Created by hrubyk on 10.08.20.
//

#include "DatabaseHelper.h"

using namespace  std;

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
