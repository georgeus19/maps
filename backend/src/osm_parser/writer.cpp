//
// Created by hrubyk on 02.09.20.
//
#include "writer.h"
using namespace std;
/*
class Writer {
    ofstream f_;
public:
    Writer(const string & file_name) {
        f_ = ofstream{};
        f_.open(file_name);
    }

    ~Writer() {
        f_.close();
    }

    void WriteCreateTableSql(const string& table_name) {
        string sql = "CREATE TABLE " + table_name + "("  \
        "id serial   NOT NULL," \
        "osm_id INT   NOT NULL," \
        "geog geography(LINESTRING)    NOT NULL," \
        "from INT     NOT NULL," \
        "to INT    NOT NULL;";
        f_ << sql << std::endl;
    }

    void WriteCreateInsertSql(const string & table_name, const string & osm_id, const string & geog, const string & from, const string & to) {
        string sql = "INSERT INTO " + table_name + " (osm_id, geog, from, to) "  \
         "VALUES (" + osm_id + ", " + geog + ", " + from + ", " + to + " );";
        f_ << sql << std::endl;
    }


};
*/
Writer::Writer(const string & file_name) {
    f_ = ofstream{};
    f_.open(file_name);
}

Writer::~Writer() {
    f_.close();
}

void Writer::WriteCreateTableSql(const string& table_name) {
    string sql = "CREATE TABLE " + table_name + "("  \
        "id serial   NOT NULL," \
        "osm_id INT   NOT NULL," \
        "geog geography(LINESTRING)    NOT NULL," \
        "from INT     NOT NULL," \
        "to INT    NOT NULL;";
    f_ << sql << std::endl;
}

void Writer::WriteCreateInsertSql(const string & table_name, const string & osm_id, const string & geog, const string & from, const string & to) {
    string sql = "INSERT INTO " + table_name + " (osm_id, geog, from, to) "  \
         "VALUES (" + osm_id + ", " + geog + ", " + from + ", " + to + " );";
    f_ << sql << std::endl;
}