//
// Created by hrubyk on 31.08.20.
//

#include "link_counter.h"
#include "graph_generator.h"
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

void CalculateNodeLinks(index_type & node_index) {
    auto otypes = osmium::osm_entity_bits::way; //osmium::osm_entity_bits::node |
    osmium::io::Reader reader{kInputPath, otypes};

    LinkCounter link_handler{node_index};
    osmium::apply(reader, link_handler);
    reader.close();
    std::cout << "Node links calculated" << std::endl;
}



int main() {
    auto otypes = osmium::osm_entity_bits::node | osmium::osm_entity_bits::way;
    osmium::io::Reader reader{kInputPath, otypes};
    string index_type_name = "sparse_mem_map";
    string table_name = "edges";
    //const auto& map_factory = osmium::index::SparseMemMapFactory<osmium::unsigned_object_id_type, size_t>::instance();
    using location_index_type = osmium::index::map::SparseMemArray<osmium::unsigned_object_id_type, osmium::Location>;
    using location_handler_type = osmium::handler::NodeLocationsForWays<location_index_type>;

    location_index_type location_index;
    location_handler_type location_handler{location_index};

    index_type node_index;
    CalculateNodeLinks(node_index);
    osmium::geom::WKBFactory<> factory{osmium::geom::wkb_type::ewkb, osmium::geom::out_type::hex};
    osmium::geom::WKTFactory<> factoryWKT{};
    FileWriter sql_writer{kOutputPath};
    GraphGenerator<osmium::geom::WKTFactory<>> graph_generator_handler{node_index, factoryWKT, sql_writer, table_name};
    //GraphGenerator<osmium::geom::WKBFactory<>> graph_generator_handler{node_index, factory};
    osmium::apply(reader, location_handler, graph_generator_handler);
    reader.close();
}