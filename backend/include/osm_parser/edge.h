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

    /**
     * Define index type which will be used throughout the osm_parser target.
     */
    using unsigned_id_type = osmium::unsigned_object_id_type;

    /**
     * Class edge is a routing graph edge. It represents a part of OSM way or a whole way with tag "highway".
     * All OSM "highway" ways are converted to one or multiple edges so that a routing graph can be created (list of edges).
     * The primary reason of this class is to store edge data which will be later written to a file (resp. db).
     */
    class Edge {
    public:
        /**
         * Id read from osm xml file. Since ways are split to edges it is not unique.
         */
        unsigned_id_type osm_id_;

        /**
         * Unique edge id.
         */
        uint64_t uid_;

        /**
         * Geography read from osm xml file representing area of this edge on map.
         */
        std::string geography_;

        /**
         * The origin of this edge.
         */
        unsigned_id_type from_;

        /**
         * The destination of this edge.
         */
        unsigned_id_type to_;

        Edge(unsigned_id_type osm_id, uint64_t uid, std::string &&geography, unsigned_id_type from, unsigned_id_type to);

        Edge(unsigned_id_type osm_id, uint64_t uid_, const std::string & geography, unsigned_id_type from, unsigned_id_type to);

        Edge(unsigned_id_type osm_id, uint64_t uid_, unsigned_id_type from, unsigned_id_type to);

        /**
         * Provide casts to string since it is the main use case of this class.
         */
        std::string get_osm_id() const;

        std::string get_uid() const;

        const std::string &get_geography() const;

        void set_geography(std::string &&geog);

        std::string get_from() const;

        std::string get_to() const;

    };
}

#endif //BACKEND_EDGE_H

