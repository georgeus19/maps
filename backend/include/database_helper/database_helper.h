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
    void CreateGraphTable(const std::string & sql);

    void SearchFor(const std::string & s);

    void FindClosestEdge(double lon, double lat, const std::string & table_name);

    void LoadGraph(double lon, double lat, double radius, const std::string & table_name);

    std::string MakeSTPoint(double lon, double lat);

};

#endif //BACKEND_DATABASEHELPER_H