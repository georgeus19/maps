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

    osmium::unsigned_object_id_type  id = way.id();

    for (const osmium::NodeRef& nr : way.nodes()) {

        osmium::unsigned_object_id_type node_id = nr.positive_ref();

        size_t value = nodes_ptr_.get_noexcept(node_id);
        bool is_in_index = value == osmium::index::empty_value<size_t>();
        if (is_in_index) {
            nodes_ptr_.set(node_id,1);
        } else {
            nodes_ptr_.set(node_id,value + 1);
        }
    }
}
