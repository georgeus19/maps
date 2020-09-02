//
// Created by hrubyk on 31.08.20.
//

#include "osm_parser.h"
#include <osmium/io/any_input.hpp>
#include "DatabaseHelper.h"
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


using namespace std;
// const string kInputPath = "/home/hrubyk/Downloads/czech-republic-latest.osm.pbf";
const string kInputPath = "/home/hrubyk/Downloads/luxembourg-latest.osm.pbf";
const string kOutputPath = "/home/hrubyk/projects/maps/backend/graph.sql";
static size_t size_tDefault;
using index_type = osmium::index::map::SparseMemMap<osmium::unsigned_object_id_type, size_t>;

class Writer {
    ofstream f_;
public:
    Writer(const string & file_name) {
        f_ = ofstream{};
        f_.open(file_name);
    }

    ~Writer() {
        f_.close();
    }

    void WriteCreateTableSql(const string& table_name) {
        string sql = "CREATE TABLE " + table_name + "("  \
        "id serial   NOT NULL," \
        "osm_id INT   NOT NULL," \
        "geog geography(LINESTRING)    NOT NULL," \
        "from INT     NOT NULL," \
        "to INT    NOT NULL;";
        f_ << sql << std::endl;
    }

    void WriteCreateInsertSql(const string & table_name, const string & osm_id, const string & geog, const string & from, const string & to) {
        string sql = "INSERT INTO " + table_name + " (osm_id, geog, from, to) "  \
         "VALUES (" + osm_id + ", " + geog + ", " + from + ", " + to + " );";
        f_ << sql << std::endl;
    }


};


class LinkCounter : public osmium::handler::Handler {
    index_type & nodes_ptr_;
public:

    LinkCounter(index_type & index_ptr) : nodes_ptr_(index_ptr) {}

    void way(const osmium::Way& way) {
        // Skip invalid ways.
        // if (way.id() <= 0) {
        //     return;
        // }

        // Skip all ways that are not roads.
        if (way.tags().get_value_by_key("highway") == nullptr) {
            return;
        }


        // std::cout << "way " << way.id() << '\n';

        osmium::unsigned_object_id_type  id = way.id();

        for (const osmium::NodeRef& nr : way.nodes()) {
            // if (nr.ref() > 0) {

                osmium::unsigned_object_id_type node_id = nr.positive_ref();
                if (way.id() == 4015956) {
                    //std::cout << "node size" << way.nodes().size() << std::endl;
                }
                if (node_id == 73014444146) {
                    std::cout << "HELLO\n";
                }

                size_t value = nodes_ptr_.get_noexcept(node_id);
                // std::cout << value << std::endl;
                bool is_in_index = value == osmium::index::empty_value<size_t>();
                if (is_in_index) {
                    nodes_ptr_.set(node_id,1);
                } else {
                    nodes_ptr_.set(node_id,value + 1);
                }
                // std::cout << "Node_index_size "<< nodes_ptr_.size() << std::endl;
                // std::cout << "AFTER INCREMENT " << nodes_ptr_.get_noexcept(node_id) << std::endl;
            // }
        }
    }

    void node(const osmium::Node& node) {
        // Skip invalid nodes.
        /*
        if (node.id() <= 0) {
            return;
        }

        nodes_ptr_.set(node.id(), 0);
        std::cout << "node " << node.id() << '\n';
        */
    }
};
template <typename GeomFactory>
class GraphGenerator : public osmium::handler::Handler {
    index_type & nodes_ptr_;
    GeomFactory factory_;
    Writer writer_;

    using const_nodelist_iterator = osmium::WayNodeList::const_iterator;

public:

    GraphGenerator(index_type & index_ptr, const GeomFactory & factory) : nodes_ptr_(index_ptr), factory_{factory}, writer_{kOutputPath} {}

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
        const_nodelist_iterator it1 = nodes.cbegin();
        const_nodelist_iterator it2 = nodes.cend();
        SaveEdge(it1, it2);

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
                SaveEdge(first, ++to);
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
            SaveEdge(first, second);
        }
        first = second;
    }

private:

    void SaveEdge(const_nodelist_iterator & from, const_nodelist_iterator & to) {
        auto&& linestring = CreateLineString(from, to);
        // std::cout << linestring << std::endl;
        // Write sql command to insert row.
        //writer_.WriteCreateInsertSql()
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



void CalculateNodeLinks(index_type & node_index) {
    auto otypes = osmium::osm_entity_bits::way; //osmium::osm_entity_bits::node |
    osmium::io::Reader reader{kInputPath, otypes};


    LinkCounter link_handler{node_index};
    osmium::apply(reader, link_handler);
    reader.close();
    std::cout << "Node links calculated" << std::endl;
}



int main() {
/*
    auto otypes = osmium::osm_entity_bits::node | osmium::osm_entity_bits::way;
    osmium::io::Reader reader{"input.osm.pbf", otypes};

    namespace map = osmium::index::map;
    using location index_type = map::SparseMemArray<osmium::unsigned_object_id_type, osmium::Location>;
    using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>;

    location_index_type location_index;
    location_handler_type location_handler{index};

    osmium::apply(reader, location_handler, handler);
    reader.close();
*/

    auto otypes = osmium::osm_entity_bits::node | osmium::osm_entity_bits::way;
    osmium::io::Reader reader{kInputPath, otypes};
    string index_type_name = "sparse_mem_map";
    //const auto& map_factory = osmium::index::SparseMemMapFactory<osmium::unsigned_object_id_type, size_t>::instance();
    using location_index_type = osmium::index::map::SparseMemArray<osmium::unsigned_object_id_type, osmium::Location>;
    using location_handler_type = osmium::handler::NodeLocationsForWays<location_index_type>;

    location_index_type location_index;
    location_handler_type location_handler{location_index};

    index_type node_index;
    CalculateNodeLinks(node_index);
    osmium::geom::WKBFactory<> factory{osmium::geom::wkb_type::ewkb, osmium::geom::out_type::hex};
    osmium::geom::WKTFactory<> factoryWKT{};
    GraphGenerator<osmium::geom::WKTFactory<>> graph_generator_handler{node_index, factoryWKT};
    //GraphGenerator<osmium::geom::WKBFactory<>> graph_generator_handler{node_index, factory};
    osmium::apply(reader, location_handler, graph_generator_handler);
    reader.close();
}

/*
int main() {
    osmium::io::File input_file{kInputPath}; // PBF format
    osmium::io::Reader reader{input_file};

    return 1;
}
 */