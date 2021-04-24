#ifndef BACKEND_CH_VERTEX_H
#define BACKEND_CH_VERTEX_H
#include "routing/edges/basic_edge.h"
#include "routing/vertices/edge_range_vertex.h"
#include <vector>
#include <limits>
#include <functional>
#include "routing/exception.h"

namespace routing {

template <typename Edge, typename EdgeRange>
class CHVertex : public BasicVertex<Edge, EdgeRange> {
public:
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

    CHVertex(unsigned_id_type osm_id, EdgeRange&& edges, unsigned_id_type ordering_rank);

    CHVertex(unsigned_id_type osm_id, EdgeRange&& edges);

    CHVertex(const CHVertex<Edge, EdgeRange>& other) = default;
    CHVertex(CHVertex<Edge, EdgeRange>&& other) = default;
    CHVertex& operator=(const CHVertex& other) = default;
    CHVertex& operator=(CHVertex&& other) = default;
    ~CHVertex() = default;

protected:
    unsigned_id_type ordering_rank_;
};
template <typename Edge, typename EdgeRange>
CHVertex<Edge, EdgeRange>::CHVertex() : BasicVertex<Edge, EdgeRange>(), ordering_rank_() {}

template <typename Edge, typename EdgeRange>
CHVertex<Edge, EdgeRange>::CHVertex(unsigned_id_type osm_id, EdgeRange&& edges, unsigned_id_type ordering_rank)
        : BasicVertex<Edge, EdgeRange>(osm_id, std::move(edges)), ordering_rank_(ordering_rank) {}

template <typename Edge, typename EdgeRange>
CHVertex<Edge, EdgeRange>::CHVertex(unsigned_id_type osm_id, EdgeRange&& edges)
        : BasicVertex<Edge, EdgeRange>(osm_id, std::move(edges)), ordering_rank_(0) {}


}

#endif //BACKEND_CH_VERTEX_H