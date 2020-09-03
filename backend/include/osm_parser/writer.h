//
// Created by hrubyk on 02.09.20.
//

#ifndef BACKEND_WRITER_H
#define BACKEND_WRITER_H
#include <string>

#include <iostream>
#include <fstream>
#include "edge.h"
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

class IWriter{
public:
    virtual void WriteInitSql(const std::string& table_name) = 0;

    virtual void WriteEdge(const std::string & table_name, const Edge & edge) = 0;

    virtual void WriteFinishSql(const std::string& table_name) = 0;

    virtual ~IWriter() {}
};

class InsertWriter : public IWriter {
    std::ofstream f_;
public:
    InsertWriter(const std::string & file_name);

    virtual ~InsertWriter();

    void WriteInitSql(const std::string& table_name);

    void WriteEdge(const std::string & table_name, const Edge & edge);

    void WriteFinishSql(const std::string& table_name);
};

class CopyWriter : public IWriter {
    std::ofstream f_init_table_;
    std::ofstream f_data_;
    std::string data_path_;

public:
    CopyWriter(const std::string & sql_path, const std::string & data_path);

    virtual ~CopyWriter();

    void WriteInitSql(const std::string& table_name);

    void WriteEdge(const std::string & table_name, const Edge & edge);

    void WriteFinishSql(const std::string& table_name);
};

#endif //BACKEND_WRITER_H
