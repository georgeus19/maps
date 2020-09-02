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
    virtual void WriteCreateTableSql(const std::string& table_name) = 0;

    virtual void WriteCreateInsertSql(const std::string & table_name, const Edge & edge) = 0;

    virtual ~IWriter() {}
};

class FileWriter : public IWriter {
    std::ofstream f_;
public:
    FileWriter(const std::string & file_name);

    virtual ~FileWriter();

    void WriteCreateTableSql(const std::string& table_name);

    void WriteCreateInsertSql(const std::string & table_name, const Edge & edge);
};
#endif //BACKEND_WRITER_H
