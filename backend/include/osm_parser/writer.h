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
    class IWriter {
    public:
        /**
         * Write any sql script initialization before edges are written to a file.
         * @param table_name Table name that will hold the data in db.
         */
        virtual void WriteInitSql(const std::string &table_name) = 0;

        /**
         * Save edge to a file in such a form that it can be loaded to db later
         * when the sql script is invoked.
         * @param table_name Table name that will hold the data in db.
         * @param edge Edge to be saved to file.
         */
        virtual void WriteEdge(const std::string &table_name, const Edge &edge) = 0;

        /**
         * Write any necessary finish for sql script.
         * @param table_name Table name that will hold the data in db.
         */
        virtual void WriteFinishSql(const std::string &table_name) = 0;

        virtual ~IWriter() {}
    };

    /**
     * InsertWriter generates a sql script that uses sql command INSERT to load
     * each edge to database.
     *
     * The advantage is that the sql script and data can be in one file
     * but it takes a lot of time to run when there are lots of edges.
     */
    class InsertWriter : public IWriter {
        /**
         * Ofstream used for writing sql script to a destination file.
         */
        std::ofstream f_;
    public:
        /**
         * @param file_name Path and name of the file where sql script is saved.
         */
        InsertWriter(const std::string &file_name);

        ~InsertWriter() override;

        /**
         * Sql table initialization - table_creation.
         * @param table_name Table name that will hold the data in db.
         */
        void WriteInitSql(const std::string &table_name) override;

        /**
         * Write INSERT sql for given edge.
         * @param table_name Table name that will hold the data in db.
         * @param edge Edge to be saved to file.
         */
        void WriteEdge(const std::string &table_name, const Edge &edge) override;

        /**
         * Creates geo index and adds length column and computes lengths.
         * @param table_name Table name that will hold the data in db.
         */
        void WriteFinishSql(const std::string &table_name) override;
    };

    /**
     * CopyWriter generates a sql script that uses
     * COPY to load edge data to db.
     *
     * It is much faster than InsertWriter but
     * it is necessary to have a sql and data file.
     */
    class CopyWriter : public IWriter {
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
        CopyWriter(const std::string &sql_path, const std::string &data_path);

        ~CopyWriter() override;

        /**
         * Write whole sql script - Table creation, length column
         * generation, length computation, geo index creation.
         * @param table_name Table name that will hold the data in db.
         */
        void WriteInitSql(const std::string &table_name) override;

        /**
         * Write edge to a edge data file.
         * @param table_name Table name that will hold the data in db.
         * @param edge Edge to be saved to file.
         */
        void WriteEdge(const std::string &table_name, const Edge &edge) override;

        /**
         * No need to do anything since everyting can be doone in WriteInitSql.
         * @param table_name Table name that will hold the data in db.
         */
        void WriteFinishSql(const std::string &table_name) override;
    };
}
#endif //BACKEND_WRITER_H
