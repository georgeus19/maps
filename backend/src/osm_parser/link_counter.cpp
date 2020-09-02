//
// Created by hrubyk on 02.09.20.
//
#include "link_counter.h"

using namespace std;

LinkCounter::LinkCounter(index_type & index_ptr) : nodes_ptr_(index_ptr) {}

void LinkCounter::way(const osmium::Way& way) {
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

void LinkCounter::node(const osmium::Node& node) {
    // Skip invalid nodes.
    /*
    if (node.id() <= 0) {
        return;
    }

    nodes_ptr_.set(node.id(), 0);
    std::cout << "node " << node.id() << '\n';
    */
}
