//
// Created by hrubyk on 10.08.20.
//

#ifndef BACKEND_DATABASEHELPER_H
#define BACKEND_DATABASEHELPER_H
#include <string>
#include <string_view>
#include <pqxx/pqxx>
#include <iostream>

class IDatabaseHelper {

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

    void SearchFor(const std::string & s);

};

#endif //BACKEND_DATABASEHELPER_H