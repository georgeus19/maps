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
    /*
     * Class used for spliting ways into segments that contain no intersection within.
     * Final segments are written to an output file using `writer_`.
     */
    template<typename GeomFactory>
    class GraphGenerator : public osmium::handler::Handler {
        index_type &nodes_ptr_;
        GeomFactory factory_;
        IWriter &writer_;
        std::string table_name_;
        uint64_t id_counter_;

        using const_nodelist_iterator = osmium::WayNodeList::const_iterator;

    public:
        GraphGenerator(index_type &index_ptr, const GeomFactory &factory, IWriter &w, const std::string &table_name)
                : nodes_ptr_(index_ptr), factory_{factory}, writer_{w}, table_name_{table_name}, id_counter_(0) {}

        /*
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

            bool oneway = false;
            auto && oneway_value = tags.get_value_by_key("oneway");
            if (oneway_value == "yes" || oneway_value == "true" || oneway_value == "1") {
                oneway = true;
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

                size_t value = nodes_ptr_.get_noexcept(second->positive_ref());
                bool not_in_index = value == osmium::index::empty_value<size_t>();
                // All of these nodes should definitely be in the index.

                if (not_in_index) {
                    std::cout << "Node not found in link index??" << std::endl;
                    std::cout << second->positive_ref() << std::endl;
                    throw 1;
                }

                bool is_intersection = value > 1 && value != osmium::index::empty_value<size_t>();
                if (is_intersection) {
                    Edge edge{way.positive_id(), ++id_counter_, first->positive_ref(), second->positive_ref()};

                    // `second` iterator points directly to the last point
                    // of the segment. Create iterator `to` which right is
                    // after `second`.
                    const_nodelist_iterator to = second;
                    ++to;
                    SaveEdge(first, to, edge, oneway);
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
                Edge edge{way.positive_id(), ++id_counter_, first->positive_ref(), last_point->positive_ref()};
                SaveEdge(first, second, edge, oneway);
            }
            first = second;
        }

    private:
        /*
         * Saves a way segment to a file.
         *
         * @param from Iterator pointing to the first node of the segment.
         * @param to Iterator pointing to the last node of the segment.
         * @param edge Segment representing the edge without geometry.
         * @param oneway If oneway is false reciprocal segment is also saved.
         */
        void SaveEdge(const_nodelist_iterator &from, const_nodelist_iterator &to, Edge &edge, bool oneway) {
            auto &&linestring = CreateLineString(from, to);
            if (linestring != "") {
                if (oneway == false) {
                    Edge reciprocal_edge{edge.osm_id_, ++id_counter_, linestring, edge.to_, edge.from_};
                    writer_.WriteEdge(table_name_, reciprocal_edge);
                }
                edge.set_geography(std::move(linestring));
                // Write sql command to insert row.
                writer_.WriteEdge(table_name_, edge);
            }
        }

        /*
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
