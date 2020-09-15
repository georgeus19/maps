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
namespace osm_parser {

    using index_type = osmium::index::map::SparseMemMap<osmium::unsigned_object_id_type, size_t>;

    /**
     * Class derived from osmium hadler class means that three methods can be overridden
     * - node(), way(), ref() - and these methods are called when osm node, way, ref
     * element is parsed. So that it is possible to e.g. read geometries and 
     * decide what to do with them.
     * 
     * This class creates an index of all the nodes that are referenced in "highway" ways (streets, roads, ...)
     * and determines if a node is an intersection or not - it increments value each time a node is referenced.
     * So that nodes with value greater that one are intersection.
     */
    class LinkCounter : public osmium::handler::Handler {
        /**
         * Aforementioned index to keep track of how many times a node is referenced.
         * The index is large and it is used also in other classes so reference is the best option.
         */
        index_type &node_links_;
    public:

        LinkCounter(index_type &index_ptr);

        /**
         * Handling what is supposed to happen with current way. All values of osm nodes in the index that are
         * referenced from `way` are increased by one.
         */
        void way(const osmium::Way &way);
    };
}
#endif //BACKEND_LINK_COUNTER_H
