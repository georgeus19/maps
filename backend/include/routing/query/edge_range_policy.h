#ifndef ROUTING_QUERY_EDGE_RANGE_POLICY_H
#define ROUTING_QUERY_EDGE_RANGE_POLICY_H

#include "routing/edge_ranges/iterator_edge_range.h"
#include "routing/edge_ranges/vector_edge_range.h"

#include <iterator>
#include <vector>

namespace routing{
namespace query{

template <typename Edge>
class EdgeRangePolicyVectorIterator{
public:

    EdgeRangePolicyVectorIterator() {}

    IteratorEdgeRange<Edge, typename std::vector<Edge>::iterator> CreateEdgeRange(std::vector<Edge>&& edges) {
        edges_.push_back(std::move(edges));
        return IteratorEdgeRange<Edge, typename std::vector<Edge>::iterator>{edges_.back().begin(), edges_.back().end()};
    }

private:
    std::vector<std::vector<Edge>> edges_;
};

template <typename Edge>
class EdgeRangePolicyVector{
public:

    EdgeRangePolicyVector() {}

    VectorEdgeRange<Edge> CreateEdgeRange(std::vector<Edge>&& edges) {
        return VectorEdgeRange<Edge>{std::move(edges)};
    }
};


}
}


#endif // ROUTING_QUERY_EDGE_RANGE_POLICY_H