//
// Created by hrubyk on 02.09.20.
//

#ifndef BACKEND_GRAPH_GENERATOR_H
#define BACKEND_GRAPH_GENERATOR_H
#include "link_counter.h"
#include <osmium/io/any_input.hpp>
#include "DatabaseHelper.h"
#include "writer.h"
#include "edge.h"
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



template <typename GeomFactory>
class GraphGenerator : public osmium::handler::Handler {
    index_type & nodes_ptr_;
    GeomFactory factory_;
    IWriter& writer_;
    std::string table_name_;

    using const_nodelist_iterator = osmium::WayNodeList::const_iterator;

public:

    GraphGenerator(index_type & index_ptr, const GeomFactory & factory, IWriter & w, const std::string & table_name)
        : nodes_ptr_(index_ptr), factory_{factory}, writer_{w},  table_name_{table_name} {}

    void way(const osmium::Way& way) {
        // Skip all ways that are not roads.
        if (way.tags().get_value_by_key("highway") == nullptr) {
            return;
        }

        // std::cout << "way " << way.id() << '\n';

        // std::cout << "Node_index_size "<< nodes_ptr_.size() << std::endl;
        const osmium::WayNodeList & nodes = way.nodes();
        // std::cout << "nodes size " << nodes.size() << '\n';
        if (nodes.size() < 2) {
            std::cout << "Way consists of less than 2 points." << std::endl;
            throw 2;
        }
        if (way.id() == 10903355) {
            std::cout << "HELLO\n";
            for(auto&& nr : nodes) {
                std::cout << nr.positive_ref() << " " << nr.ref() << " ";
            }
            std::cout << std::endl;
        }
        {
            const_nodelist_iterator it1 = nodes.cbegin();
            const_nodelist_iterator it2 = nodes.cend();
            Edge edge{way.positive_id(), it1->positive_ref(), it2->positive_ref()};
            SaveEdge(it1, it2, edge);
        }

        const_nodelist_iterator first = nodes.cbegin();
        const_nodelist_iterator second = nodes.cbegin();
        ++second;
        size_t i = 0;
        for(; second != nodes.cend(); ++second) {
            // Skip invalid nodes. - TOOD what is such a node is a first or last one???
            // if (second->ref() < 0) continue;

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
                const_nodelist_iterator to = second;
                // Incrementing `to` since we include the intersection point in the linestring.
                ++to;
                Edge edge{way.positive_id(), first->positive_ref(), to->positive_ref()};
                SaveEdge(first, to, edge);
                first = second;
            }
            ++i;
        }

        // Create a way segment from first to the last point in `nodes`.
        // If it is an intersection, it was added in the for loop.
        // In that case `first` should point to the last point.
        // If the last one was not intersection then create
        // way segment.
        const_nodelist_iterator it = first;
        if (++it != nodes.cend()) {
            Edge edge{way.positive_id(), first->positive_ref(), second->positive_ref()};
            SaveEdge(first, second, edge);
        }
        first = second;
    }

private:

    void SaveEdge(const_nodelist_iterator & from, const_nodelist_iterator & to, Edge & edge) {
        auto&& linestring = CreateLineString(from, to);
        edge.set_geography(std::move(linestring));
        // Write sql command to insert row.
        writer_.WriteCreateInsertSql(table_name_, edge);

    }

    std::string CreateLineString(const_nodelist_iterator from, const_nodelist_iterator to) {
        factory_.linestring_start();
        size_t point_count = factory_.fill_linestring_unique(from, to);
        if (point_count < 2) {
            std::cout << "Way segment consists of less than 2 points." << std::endl;
            //throw 2;
        }
        return factory_.linestring_finish(point_count);
    }
};

#endif //BACKEND_GRAPH_GENERATOR_H
