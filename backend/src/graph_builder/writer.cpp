#include "graph_builder/writer.h"
using namespace std;
namespace graph_builder {
    
CopyWriter::CopyWriter(const std::string &sql_path, const std::string &data_path) {
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
    std::string temporary_edges_table = table_name + "_temp";
    std::string vertex_id_mapping_table = temporary_edges_table + "_vertex_mapping_temp";
    // Write COPY command that loads edges from data to table `table_name`.
    string copy = "COPY " + temporary_edges_table + " FROM '" + data_path_ + "' DELIMITER ';' CSV; ";
    // Add length column.
    string add_length_column = "ALTER TABLE " + table_name + " ADD COLUMN length REAL; ";
    // Calculate lengths.
    string fill_length_column = "UPDATE " + table_name + " set length = ST_Length(geog); ";
    // Create geo index.
    string create_index = "CREATE INDEX " + table_name + "_gix ON " + table_name + " USING GIST (geog); ";
    f_init_table_ << "BEGIN; " << std::endl;
    f_init_table_ << GetCreateTemporaryEdgesTable(temporary_edges_table) << std::endl;
    f_init_table_ << copy << std::endl;
    f_init_table_ << GetCreateVertexIdMappingTable(temporary_edges_table, vertex_id_mapping_table) << std::endl;
    f_init_table_ << GetCreateFinalEdgesTable(table_name) << std::endl;
    f_init_table_ << GetInsertToFinalTable(table_name, temporary_edges_table, vertex_id_mapping_table) << std::endl;
    f_init_table_ << GetDropTable(temporary_edges_table) << std::endl;
    f_init_table_ << GetDropTable(vertex_id_mapping_table) << std::endl;
    f_init_table_ << add_length_column << std::endl;
    f_init_table_ << fill_length_column << std::endl;
    f_init_table_ << create_index << std::endl;
    f_init_table_ << "COMMIT; " << std::endl;
}

void CopyWriter::WriteEdge(const string &table_name, const Edge &edge) {
    // Write edge to data file. Data format is CSV with ';' delimiters.
    string data = edge.get_osm_id() + ";" + edge.get_uid() + ";" + edge.get_geography() + ";" + edge.get_from() + ";" + edge.get_to() 
        + ";" + edge.get_undirected() + ";" + edge.get_highway();
    f_data_ << data << std::endl;
}

void CopyWriter::WriteFinishSql(const std::string &table_name) {
    // All done in WriteInitSql...
}

std::string CopyWriter::GetCreateTemporaryEdgesTable(const std::string& table_name) const {
    return GetDropTable(table_name) + 
        "CREATE TABLE " + table_name + "("  \
        "   osm_id BIGINT NOT NULL, " \
        "   uid INTEGER PRIMARY KEY, " \
        "   geog geography(LINESTRING) NOT NULL, " \
        "   from_node BIGINT NOT NULL, " \
        "   to_node BIGINT NOT NULL, " \
        "   undirected BOOLEAN NOT NULL, " \
        "   highway TEXT NOT NULL " \
        "); ";
}

std::string CopyWriter::GetCreateFinalEdgesTable(const std::string& table_name) const {
    return GetDropTable(table_name) + 
        "CREATE TABLE " + table_name + "("  \
        "   osm_id BIGINT NOT NULL, " \
        "   uid INTEGER PRIMARY KEY, " \
        "   geog geography(LINESTRING) NOT NULL, " \
        "   from_node INTEGER NOT NULL, " \
        "   to_node INTEGER NOT NULL, " \
        "   undirected BOOLEAN NOT NULL, " \
        "   highway TEXT NOT NULL " \
        "); ";
}

std::string CopyWriter::GetCreateVertexIdMappingTable(const std::string& edges_table, const std::string& mapping_table) const {
    return GetDropTable(mapping_table) + 
        "CREATE TABLE " + mapping_table + " ( " \
        "   osm_id BIGINT PRIMARY KEY, " \
        "   new_id BIGSERIAL NOT NULL); " \
        "INSERT INTO " + mapping_table + "(osm_id)( " \
        "   SELECT from_node as vertex_id " \
        "   FROM " + edges_table + " " \
        "   UNION " \
        "   SELECT to_node " \
        "   FROM " + edges_table + " " \
        "); ";
}

std::string CopyWriter::GetInsertToFinalTable(const std::string& final_table, const std::string& edges_table, const std::string& mapping_table) const {
    return "INSERT INTO " + final_table + "(osm_id, uid, geog, from_node, to_node, undirected, highway) " \
        " ( " \
        "   SELECT e.osm_id, e.uid, e.geog, vfrom.new_id::INTEGER, vto.new_id::INTEGER, e.undirected, e.highway " \
        "   FROM " + edges_table + " AS e INNER JOIN " + mapping_table + " AS vfrom ON from_node = vfrom.osm_id " \
        "   INNER JOIN " + mapping_table + " AS vto ON to_node = vto.osm_id " \
        " ); ";
}

std::string CopyWriter::GetDropTable(const std::string& table_name) const {
    return "DROP TABLE IF EXISTS " + table_name + "; ";
}



}