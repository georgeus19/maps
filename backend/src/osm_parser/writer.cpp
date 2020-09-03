//
// Created by hrubyk on 02.09.20.
//
#include "writer.h"
using namespace std;

InsertWriter::InsertWriter(const string & file_name) {
    f_ = ofstream{};
    f_.open(file_name);
}

InsertWriter::~InsertWriter() {
    f_.close();
}

void InsertWriter::WriteInitSql(const string& table_name) {
    string sql = "CREATE TABLE " + table_name + "("  \
        "id serial PRIMARY KEY, " \
        "osm_id BIGINT NOT NULL, " \
        "geog geography(LINESTRING) NOT NULL, " \
        "from_node BIGINT NOT NULL, " \
        "to_node BIGINT NOT NULL);";
    f_ << sql << std::endl;
}

void InsertWriter::WriteEdge(const string & table_name, const Edge & edge) {
    string sql = "INSERT INTO " + table_name + " (osm_id, geog, from_node, to_node) "  \
         "VALUES (" + edge.get_osm_id() + ", '" + edge.get_geography() + "', " + edge.get_from() + ", " + edge.get_to() + " );";
    f_ << sql << std::endl;
}

void InsertWriter::WriteFinishSql(const std::string& table_name) {
    string add_length_column = "ALTER TABLE " + table_name + " ADD COLUMN length double precision;";
    string fill_length_column = "UPDATE " + table_name + " set length = st_length(geog);";
    string create_index = "CREATE INDEX " + table_name + "_gix ON " + table_name + " USING GIST (geog);";

    f_ << add_length_column << std::endl;
    f_ << fill_length_column << std::endl;
    f_ << create_index << std::endl;
}

CopyWriter::CopyWriter(const std::string & sql_path, const std::string & data_path) {
    f_init_table_ = ofstream{};
    f_init_table_.open(sql_path);
    f_data_ = ofstream{};
    f_data_.open(data_path);
    data_path_ = data_path;
}

CopyWriter::~CopyWriter() {
    f_init_table_.close();
    f_data_.close();
}

void CopyWriter::WriteInitSql(const string& table_name) {
    string create_table = "CREATE TABLE " + table_name + "("  \
        "osm_id BIGINT NOT NULL, " \
        "geog geography(LINESTRING) NOT NULL, " \
        "from_node BIGINT NOT NULL, " \
        "to_node BIGINT NOT NULL);";
    string copy = "COPY " + table_name + " FROM '" + data_path_ + "' DELIMITER ';' CSV HEADER;";
    string add_length_column = "ALTER TABLE " + table_name + " ADD COLUMN length double precision;";
    string fill_length_column = "UPDATE " + table_name + " set length = st_length(geog);";
    string create_index = "CREATE INDEX " + table_name + "_gix ON " + table_name + " USING GIST (geog);";
    f_init_table_ << create_table << std::endl;
    f_init_table_ << copy << std::endl;
    f_init_table_ << add_length_column << std::endl;
    f_init_table_ << fill_length_column << std::endl;
    f_init_table_ << create_index << std::endl;

}

void CopyWriter::WriteEdge(const string & table_name, const Edge & edge) {
    string data = edge.get_osm_id() + "; " + edge.get_geography() + "; " + edge.get_from() + "; " + edge.get_to();
    f_data_ << data << std::endl;
}

void CopyWriter::WriteFinishSql(const std::string& table_name) {

}