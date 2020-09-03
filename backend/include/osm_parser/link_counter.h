//
// Created by hrubyk on 02.09.20.
//

#ifndef BACKEND_LINK_COUNTER_H
#define BACKEND_LINK_COUNTER_H
#include <osmium/io/any_input.hpp>
#include <osmium/io/any_output.hpp>
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

using index_type = osmium::index::map::SparseMemMap<osmium::unsigned_object_id_type, size_t>;

class LinkCounter : public osmium::handler::Handler {
    index_type & nodes_ptr_;
public:

    LinkCounter(index_type & index_ptr);

    void way(const osmium::Way& way);

    void node(const osmium::Node& node);
};

#endif //BACKEND_LINK_COUNTER_H
