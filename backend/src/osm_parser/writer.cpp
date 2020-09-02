//
// Created by hrubyk on 02.09.20.
//
#include "writer.h"
using namespace std;

FileWriter::FileWriter(const string & file_name) {
    f_ = ofstream{};
    f_.open(file_name);
}

FileWriter::~FileWriter() {
    f_.close();
}

void FileWriter::WriteCreateTableSql(const string& table_name) {
    string sql = "CREATE TABLE " + table_name + "("  \
        "id serial   NOT NULL," \
        "osm_id INT   NOT NULL," \
        "geog geography(LINESTRING)    NOT NULL," \
        "from INT     NOT NULL," \
        "to INT    NOT NULL;";
    f_ << sql << std::endl;
}

void FileWriter::WriteCreateInsertSql(const string & table_name, const Edge & edge) {
    string sql = "INSERT INTO " + table_name + " (osm_id, geog, from, to) "  \
         "VALUES (" + edge.get_osm_id() + ", " + edge.get_geography() + ", " + edge.get_from() + ", " + edge.get_to() + " );";
    f_ << sql << std::endl;
}