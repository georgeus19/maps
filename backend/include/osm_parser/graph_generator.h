#ifndef BACKEND_GRAPH_GENERATOR_H
#define BACKEND_GRAPH_GENERATOR_H
#include "osm_parser/link_counter.h"
#include <osmium/io/any_input.hpp>

#include "osm_parser/writer.h"
#include "osm_parser/edge.h"
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
    /**
     * Class derived from osmium hadler class means that three methods can be overridden
     * - node(), way(), ref() - and these methods are called when osm node, way, ref
     * element is parsed. So that it is possible to e.g. read geometries and 
     * decide what to do with them.
     * 
     * Class used for spliting ways into segments (=graph edges) that contain no intersection within.
     * Final segments are written to an output file using `writer_`. The file is a sql script
     * that can load the segments into db.
     */
    template<typename GeomFactory>
    class GraphGenerator : public osmium::handler::Handler {
        /**
         * A index type provided by osmium library.
         * This field contains for each node a number of how many ways it is part of.
         * So if the number is greater than one it is an intersection.
         */
        index_type &node_links_;

        /**
         * Osmium factory used to create wkt or wkb format geometries from lon lat coordinates.
         * These geometries can be further used with postgis.
         */
        GeomFactory factory_;

        /**
         * Writer writes each way (resp. its parts=edges) to a file with its geometry, etc...
         * It generates sql script that can be used to load edges to db.
         */
        IWriter &writer_;

        /**
         * Name of db table which should be created to hold the data.
         * Only in sql script. No db table is created now.
         */
        std::string table_name_;

        /**
         * Field that is incremented for each new edge.
         * It is a source of unique ids for edges.
         */
        uint64_t id_counter_;

        using const_nodelist_iterator = osmium::WayNodeList::const_iterator;

    public:
        GraphGenerator(index_type &node_links, const GeomFactory &factory, IWriter &w, const std::string &table_name)
                : node_links_(node_links), factory_{factory}, writer_{w}, table_name_{table_name}, id_counter_(5) {}

        /**
         * Handles operation for current read way.
         * If way is highway it is split into segments (=edges)
         * segments' geometries (linestring) are created.
         * Way segment is the longest possible part of way
         * that has no intersection in the middle nodes.
         * Segments are saved in a output file using `writer_.
         *
         * If the way is not tagged as oneway,
         * reciprocal segments are also created and saved.
         *
         * @param way Current read way from input file.
         */
        void way(const osmium::Way &way) {
            auto && tags = way.tags();
            // Skip all ways that are not roads.
            if (tags.get_value_by_key("highway") == nullptr) {
                return;
            }

            bool undirected = true;
            auto && oneway_value = tags.get_value_by_key("oneway");
            if (oneway_value == "yes" || oneway_value == "true" || oneway_value == "1") {
                undirected = false;
            }

            const osmium::WayNodeList &nodes = way.nodes();
            if (nodes.size() < 2) {
                std::cout << "Way consists of less than 2 points." << std::endl;
                return;
            }

            const_nodelist_iterator first = nodes.cbegin();
            const_nodelist_iterator second = nodes.cbegin();
            ++second;
            for (; second != nodes.cend(); ++second) {

                size_t value = node_links_.get_noexcept(second->positive_ref());
                bool not_in_index = value == osmium::index::empty_value<size_t>();
                // All of these nodes should definitely be in the index.

                if (not_in_index) {
                    std::cout << "Node not found in link index??" << std::endl;
                    std::cout << second->positive_ref() << std::endl;
                    throw 1;
                }

                bool is_intersection = value > 1 && value != osmium::index::empty_value<size_t>();
                if (is_intersection) {
                    Edge edge{way.positive_id(), ++id_counter_, first->positive_ref(), second->positive_ref(), undirected};

                    // `second` iterator points directly to the last point
                    // of the segment. Create iterator `to` which right is
                    // after `second`.
                    const_nodelist_iterator to = second;
                    ++to;
                    SaveEdge(first, to, edge);
                    first = second;
                }
            }

            // Create a way segment from first to the last point in `nodes`.
            // If it is an intersection, it was added in the for loop.
            // In that case `first` should point to the last point.
            // If the last one was not intersection then create
            // way segment.
            const_nodelist_iterator it = first;
            if (++it != nodes.cend()) {

                // `second` now points to end() of the collection.
                // Retrieve osm_id of the last point of the segment
                // by using temporary iterator `last_point`
                // which points directly before `second`.
                const_nodelist_iterator last_point = second;
                --last_point;
                Edge edge{way.positive_id(), ++id_counter_, first->positive_ref(), last_point->positive_ref(), undirected};
                SaveEdge(first, second, edge);
            }
            first = second;
        }

    private:
        /**
         * Saves a way segment to a file.
         *
         * @param from Iterator pointing to the first node of the segment.
         * @param to Iterator pointing to the last node of the segment.
         * @param edge Segment representing the edge without geometry.
         * @param undirected If undirected is false reciprocal segment is also saved.
         */
        void SaveEdge(const_nodelist_iterator &from, const_nodelist_iterator &to, Edge &edge) {
            if (edge.from_ == edge.to_) {
                return; // Running stadium lines, circle around oil station -> useless for routing..
            }
            auto &&linestring = CreateLineString(from, to);
            if (linestring != "") {
                edge.set_geography(std::move(linestring));
                // Write sql command to insert row.
                writer_.WriteEdge(table_name_, edge);
            }
        }

        /**
         * Creates a wkt linestring of a segment represented by given iterators.
         *
         * @param from Iterator pointing to the first node of the segment.
         * @param to Iterator pointing to the last node of the segment.
         */
        std::string CreateLineString(const_nodelist_iterator from, const_nodelist_iterator to) {
            factory_.linestring_start();
            size_t point_count = factory_.fill_linestring_unique(from, to);
            if (point_count < 2) {
                std::cout << "Way segment consists of less than 2 points." << std::endl;
                factory_.linestring_finish(point_count);
                return "";
            }
            return factory_.linestring_finish(point_count);
        }
    };
}
#endif //BACKEND_GRAPH_GENERATOR_H
