//
// Created by hrubyk on 02.09.20.
//

#ifndef BACKEND_EDGE_H
#define BACKEND_EDGE_H

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
namespace osm_parser {

    using unsigned_id_type = osmium::unsigned_object_id_type;

    class Edge {
    public:
        unsigned_id_type osm_id_;
        std::string geography_;
        unsigned_id_type from_;
        unsigned_id_type to_;

        Edge(unsigned_id_type osm_id, std::string &&geography, unsigned_id_type from, unsigned_id_type to);

        Edge(unsigned_id_type osm_id, const std::string & geography, unsigned_id_type from, unsigned_id_type to);

        Edge(unsigned_id_type osm_id, unsigned_id_type from, unsigned_id_type to);

        std::string get_osm_id() const;

        const std::string &get_geography() const;

        void set_geography(std::string &&geog);

        std::string get_from() const;

        std::string get_to() const;

    };
}

#endif //BACKEND_EDGE_H

