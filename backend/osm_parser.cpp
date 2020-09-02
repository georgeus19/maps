//
// Created by hrubyk on 31.08.20.
//

#include "xml_parser.h"

#include <osmium/io/file.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/io/any_output.hpp>
#include <string>
using namespace std;

const string kInputPath = "/home/hrubyk/Downloads/cz.osm.pbf";

#include <iostream>

#include <osmium/handler.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/osm/node.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/visitor.hpp>
#include <osmium/index/sparse_file_array.hpp>

class LinkCounter : public osmium::handler::Handler {
public:
    void way(const osmium::Way& way) {
        // Skip all ways that are not roads.
        if (way.tags().get_value_by_key("highway") == nullptr) {
            return;
        }

        std::cout << "way " << way.id() << '\n';
        for (const osmium::Tag& t : way.tags()) {
            std::cout << t.key() << "=" << t.value() << '\n';
        }
    }

    void node(const osmium::Node& node) {
        std::cout << "node " << node.id() << '\n';
    }
};

int main() {
    auto otypes = osmium::osm_entity_bits::node | osmium::osm_entity_bits::way;
    osmium::io::Reader reader{kInputPath, otypes};
    LinkCounter handler;
    osmium::apply(reader, handler);
    reader.close();
}

/*
int main() {
    osmium::io::File input_file{kInputPath}; // PBF format
    osmium::io::Reader reader{input_file};

    return 1;
}
 */