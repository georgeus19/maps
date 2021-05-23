#ifndef BACKEND_WRITER_H
#define BACKEND_WRITER_H
#include <string>
#include <iostream>
#include <fstream>
#include "osm_parser/edge.h"
#include <osmium/handler.hpp>
#include <osmium/osm/node.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/visitor.hpp>
#include <osmium/index/map/sparse_file_array.hpp>
#include <osmium/index/map/sparse_mem_map.hpp>
#include <osmium/index/map/sparse_mem_array.hpp>
#include <osmium/geom/wkb.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/geom/wkt.hpp>
namespace osm_parser {
    
/**
 * Interface for osm_parser writers that can be used
 * to write a sql script to a file that generates an edgelist.
 *
 * A part of the script should also be to generate an geo index
 * over the edgelist based on the geometries and adding length
 * column as well as counting lengths for edges.
 */
class Writer {
public:
    /**
     * Write any sql script initialization before edges are written to a file.
     * @param table_name Table name that will hold the data in db.
     */
    virtual void WriteInitSql(const std::string& table_name) = 0;

    /**
     * Save edge to a file in such a form that it can be loaded to db later
     * when the sql script is invoked.
     * @param table_name Table name that will hold the data in db.
     * @param edge Edge to be saved to file.
     */
    virtual void WriteEdge(const std::string& table_name, const Edge& edge) = 0;

    /**
     * Write any necessary finish for sql script.
     * @param table_name Table name that will hold the data in db.
     */
    virtual void WriteFinishSql(const std::string& table_name) = 0;

    virtual ~Writer() {}
};

/**
 * CopyWriter generates a sql script that uses
 * COPY to load edge data to db.
 *
 */
class CopyWriter : public Writer {
    /**
     * Ofstream used to write sql script to a file.
     */
    std::ofstream f_init_table_;

    /**
     * Ofstream used to write edge data to a file.
     */
    std::ofstream f_data_;

    /**
     * Path of file with edge data.
     */
    std::string data_path_;

public:
    /**
     * @param sql_path Path and name of the file where sql script is saved.
     * @param data_path Path and name of the file where edge data is saved.
     */
    CopyWriter(const std::string& sql_path, const std::string& data_path);

    ~CopyWriter() override;

    /**
     * Write whole sql script - Table creation, length column
     * generation, length computation, geo index creation.
     * @param table_name Table name that will hold the data in db.
     */
    void WriteInitSql(const std::string& table_name) override;

    /**
     * Write edge to a edge data file.
     * @param table_name Table name that will hold the data in db.
     * @param edge Edge to be saved to file.
     */
    void WriteEdge(const std::string& table_name, const Edge& edge) override;

    /**
     * No need to do anything since everyting can be doone in WriteInitSql.
     * @param table_name Table name that will hold the data in db.
     */
    void WriteFinishSql(const std::string& table_name) override;

private:
    std::string GetCreateEdgesTable(const std::string& table_name) const;

    std::string GetCreateVertexIdMappingTable(const std::string& edges_table, const std::string& mapping_table) const;

    std::string GetInsertToFinalTable(const std::string& final_table, const std::string& edges_table, const std::string& mapping_table) const;

    std::string GetDropTable(const std::string& table_name) const;
};



}
#endif //BACKEND_WRITER_H
