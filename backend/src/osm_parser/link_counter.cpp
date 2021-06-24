#include "osm_parser/link_counter.h"

using namespace std;
namespace osm_parser {
    LinkCounter::LinkCounter(index_type &node_links) : node_links_(node_links) {}

    void LinkCounter::way(const osmium::Way &way) {
        // Skip invalid ways.
        // if (way.id() <= 0) {
        //     return;
        // }

        // Skip all ways that are not roads.
        if (way.tags().get_value_by_key("highway") == nullptr) {
            return;
        }

        for (const osmium::NodeRef &nr : way.nodes()) {

            osmium::unsigned_object_id_type node_id = nr.positive_ref();

            size_t value = node_links_.get_noexcept(node_id);
            // If a node is not found in the index osmium library
            // returns osmium::index::empty_value<T>() in get_noexcept().
            bool not_in_index = value == osmium::index::empty_value<size_t>();
            if (not_in_index) {
                node_links_.set(node_id, 1);
            } else {
                node_links_.set(node_id, value + 1);
            }
        }
    }
}
