//
// Created by hrubyk on 02.09.20.
//

#ifndef BACKEND_WRITER_H
#define BACKEND_WRITER_H
#include <string>

#include <iostream>
#include <fstream>

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

class Writer {
    std::ofstream f_;
public:
    Writer(const std::string & file_name);

    ~Writer();

    void WriteCreateTableSql(const std::string& table_name);

    void WriteCreateInsertSql(const std::string & table_name, const std::string & osm_id, const std::string & geog, const std::string & from, const std::string & to);
};
#endif //BACKEND_WRITER_H
