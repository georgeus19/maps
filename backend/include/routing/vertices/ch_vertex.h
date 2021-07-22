#ifndef ROUTING_VERTICES_CH_VERTEX_H
#define ROUTING_VERTICES_CH_VERTEX_H
#include "routing/edges/basic_edge.h"
#include "routing/exception.h"
#include "routing/types.h"

#include <vector>
#include <limits>
#include <functional>

namespace routing {

/**
 * Vertex for ContractionHierarchies algorithm.
 */
template <typename Edge, typename ERange>
class CHVertex : public BasicVertex<Edge, ERange> {
public:
    using EdgeRange = ERange;

    inline unsigned_id_type get_ordering_rank() const {
        return ordering_rank_;
    }

    inline void set_ordering_rank(unsigned_id_type ordering_rank) {
        ordering_rank_ = ordering_rank;
    }

    bool IsContracted() const {
        return ordering_rank_ > 0;
    }

    CHVertex();

    CHVertex(unsigned_id_type osm_id, ERange&& edges, unsigned_id_type ordering_rank);

    CHVertex(unsigned_id_type osm_id, ERange&& edges);

    CHVertex(const CHVertex<Edge, ERange>& other) = default;
    CHVertex(CHVertex<Edge, ERange>&& other) = default;
    CHVertex& operator=(const CHVertex& other) = default;
    CHVertex& operator=(CHVertex&& other) = default;
    ~CHVertex() = default;

protected:
    unsigned_id_type ordering_rank_;
};
template <typename Edge, typename ERange>
CHVertex<Edge, ERange>::CHVertex() : BasicVertex<Edge, ERange>(), ordering_rank_(0) {}

template <typename Edge, typename ERange>
CHVertex<Edge, ERange>::CHVertex(unsigned_id_type osm_id, ERange&& edges, unsigned_id_type ordering_rank)
        : BasicVertex<Edge, ERange>(osm_id, std::move(edges)), ordering_rank_(ordering_rank) {}

template <typename Edge, typename ERange>
CHVertex<Edge, ERange>::CHVertex(unsigned_id_type osm_id, ERange&& edges)
        : BasicVertex<Edge, ERange>(osm_id, std::move(edges)), ordering_rank_(0) {}


}

#endif //ROUTING_VERTICES_CH_VERTEX_H