/*
 * This cpp file has an entrypoint of target graph_builder.
 * It takes input xml file path, table name, output sql file, output data file
 * as cmd arguments. Xml file is parsed and sql to create routing graph is generated
 * to output files.
 *
 * Libosmium library is used for parsing OSM data. It is quite convenient since
 * normal xml libraries work with whole xml tree and xml data are to large
 * to do that.
 *
 * Libosmium library provides a way to define handlers which say what should
 * be done with any read osm element - node, way, ref.
 */
#include "osm_parser/link_counter.h"
#include "osm_parser/graph_generator.h"
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
namespace osm_parser {
    using location_index_type = osmium::index::map::SparseMemArray<osmium::unsigned_object_id_type, osmium::Location>;
    using location_handler_type = osmium::handler::NodeLocationsForWays<location_index_type>;

    /**
     * Parse input xml file and identify which nodes are
     * intersection and which are not. Results are saved in
     * `node_index`.
     * @param node_index Index which holds values for each node that is referenced
     *                   by osm "highway" way (=road, street, ...). It increments
     *                   value of certain nodes when it is referenced. So when
     *                   value > 1 then node is an intersection.
     * @param input_path Osm xml data path.
     */
    void CalculateNodeLinks(index_type &node_index, const std::string &input_path) {
        // Parse only way elements.
        auto otypes = osmium::osm_entity_bits::way;
        osmium::io::Reader reader{input_path, otypes};

        LinkCounter link_handler{node_index};
        // Parse input file via reader and apply LinkCounter handler.
        osmium::apply(reader, link_handler);
        reader.close();
        std::cout << "Node links calculated." << std::endl;
    }

    /**
     * Generate a edgelist routing graph sql script to output files.
     * This sql script is capable of loading the graph into database table.
     * Postgis extension must be enabled in the database.
     *
     * Split each way that has intersection nodes in itself to segments (edges)
     * that contain no intersection in middle nodes (endpoints can be intersections).
     * Create geometries for these segments and save them to file - see osm_parser/graph_generator.h
     * for more information.
     *
     * @param node_index Index of all references nodes by "highway" ways - identifies intersection nodes.
     * @param input_path Input path of osm xml data file.
     * @param table_name Name of table that where sql script creates graph.
     * @param output_sql_path Path of file where sql script is saved.
     * @param output_data_path Path of file where data is stored if there is any.
     */
    void GenerateGraph(index_type &node_index, const std::string &input_path, const string &table_name,
                       const string &output_sql_path, const string &output_data_path) {
        // Read only osm nodes and ways.
        auto otypes = osmium::osm_entity_bits::node | osmium::osm_entity_bits::way;
        osmium::io::Reader reader{input_path, otypes};
        location_index_type location_index;
        location_handler_type location_handler{location_index};

        //osmium::geom::WKBFactory<> factoryWKB{osmium::geom::wkb_type::ewkb, osmium::geom::out_type::hex};
        // Geometries in output files will be of WKT format.
        osmium::geom::WKTFactory<> factoryWKT{};
        // InsertWriter writer{output_sql_path};
        // Use CopyWriter for writing to ouput files -> faster.
        CopyWriter writer{output_sql_path, output_data_path};
        writer.WriteInitSql(table_name);

        GraphGenerator<osmium::geom::WKTFactory<>> graph_generator_handler{node_index, factoryWKT, writer, table_name};
        //GraphGenerator<osmium::geom::WKBFactory<>> graph_generator_handler{node_index, factoryWKB, writer, table_name};

        // Location handler enables to read lon lat coordinates of nodes referenced by ways.
        osmium::apply(reader, location_handler, graph_generator_handler);
        writer.WriteFinishSql(table_name);
        reader.close();
    }
}

// Target graph_builder executable entrypoint.
int main(int argc, const char *argv[]) {
    using namespace osm_parser;
    if (argc != 5) {
        std::cout << "There must be 4 arguments: input_path table_name output_sql_path output_data_path."
                  << std::endl;
        return 1;
    }

    // Parse cmd arguments.
    index_type node_index;
    std::string input_path = argv[1];
    std::string table_name = argv[2];
    std::string output_sql_path = argv[3];
    std::string output_data_path = argv[4];
    // Identify intersections.
    CalculateNodeLinks(node_index, input_path);
    // Split ways and save graph edges to output files.
    GenerateGraph(node_index, input_path, table_name, output_sql_path, output_data_path);
    return 0;
}