#ifndef ROUTING_QUERY_ROUTE_H
#define ROUTING_QUERY_ROUTE_H

#include "routing/edges/basic_edge.h"
#include "routing/types.h"

#include "routing/profile/preference_index.h"

#include <vector>
#include <string>

namespace routing{
namespace query{

/**
 * 
 */
template <typename Edge>
class Route {
public:

    Route(std::vector<Edge>&& e, std::string&& g);

    const std::string& get_geometry() const;

    float GetLength(profile::PreferenceIndex* index) const;

private:
    std::vector<Edge> edges_;
    std::string geometry_;
};

template <typename Edge>
Route<Edge>::Route(std::vector<Edge>&& e, std::string&& g) : edges_(std::move(e)), geometry_(std::move(g)) {}

template <typename Edge>
const std::string& Route<Edge>::get_geometry() const {
    return geometry_;
}


template <typename Edge>
float Route<Edge>::GetLength(profile::PreferenceIndex* index) const {
    assert(edges_.size() >= 2);

    auto it = edges_.begin();
    float length = it->get_length();
    ++it;

    auto end_it = edges_.end();
    --end_it;
    length += end_it->get_length();

    for(; it != end_it; ++it) {
        length += index->GetOriginal(it->get_uid());
    }
    return length;
}


}
}
#endif // ROUTING_QUERY_ROUTE_H