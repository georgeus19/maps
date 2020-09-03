//
// Created by hrubyk on 31.08.20.
//

#include "link_counter.h"
#include "graph_generator.h"
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
#include <osmium/geom/mercator_projection.hpp>
using namespace std;

using location_index_type = osmium::index::map::SparseMemArray<osmium::unsigned_object_id_type, osmium::Location>;
using location_handler_type = osmium::handler::NodeLocationsForWays<location_index_type>;

void CalculateNodeLinks(index_type & node_index, const std::string & input_path) {
    auto otypes = osmium::osm_entity_bits::way; //osmium::osm_entity_bits::node |
    osmium::io::Reader reader{input_path, otypes};

    LinkCounter link_handler{node_index};
    osmium::apply(reader, link_handler);
    reader.close();
    std::cout << "Node links calculated" << std::endl;
}

void GenerateGraph(index_type & node_index, const std::string & input_path, const string & table_name,  const string & output_sql_path, const string & output_data_path) {
    auto otypes = osmium::osm_entity_bits::node | osmium::osm_entity_bits::way;
    osmium::io::Reader reader{input_path, otypes};
    location_index_type location_index;
    location_handler_type location_handler{location_index};

    //osmium::geom::WKBFactory<> factoryWKB{osmium::geom::wkb_type::ewkb, osmium::geom::out_type::hex};
    osmium::geom::WKTFactory<> factoryWKT{};
    // InsertWriter writer{output_sql_path};
    CopyWriter writer{output_sql_path, output_data_path};
    writer.WriteInitSql(table_name);

    GraphGenerator<osmium::geom::WKTFactory<>> graph_generator_handler{node_index, factoryWKT, writer, table_name};
    //GraphGenerator<osmium::geom::WKBFactory<>> graph_generator_handler{node_index, factoryWKB, writer, table_name};

    osmium::apply(reader, location_handler, graph_generator_handler);
    writer.WriteFinishSql(table_name);
    reader.close();
}

int main(int argc, const char* argv[]) {
    if (argc != 5) {
        std::cout << "There must be 4 arguments: input_path table_name output_sql_path output_data_path." << std::endl;
        return 1;
    }

    index_type node_index;
    std::string input_path = argv[1];
    std::string table_name = argv[2];
    std::string output_sql_path = argv[3];
    std::string output_data_path = argv[4];
    CalculateNodeLinks(node_index, input_path);
    GenerateGraph(node_index, input_path, table_name, output_sql_path, output_data_path);
    return 0;
}