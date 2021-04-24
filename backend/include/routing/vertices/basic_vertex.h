#ifndef BACKEND_BASIC_VERTEX_H
#define BACKEND_BASIC_VERTEX_H
#include "routing/edges/basic_edge.h"
#include <vector>
#include <limits>
#include <functional>
#include "routing/exception.h"

namespace routing {

/**
 * BasicVertex is a routing graph vertex with only the most general properties.
 */
template <typename Edge, typename EdgeRange>
class BasicVertex {

public:
    inline unsigned_id_type get_osm_id() const {
        return osm_id_;
    } 

    inline EdgeRange& get_edges() {
        return edges_;
    }

    BasicVertex();
    BasicVertex(unsigned_id_type osm_id, EdgeRange&& edges);

    BasicVertex(const BasicVertex<Edge, EdgeRange>& other) = default;
    BasicVertex(BasicVertex<Edge, EdgeRange>&& other) = default;
    BasicVertex& operator=(const BasicVertex& other) = default;
    BasicVertex& operator=(BasicVertex&& other) = default;
    virtual ~BasicVertex() = default;

    unsigned_id_type GetPreviousDefaultValue() const;

    void ResetRoutingProperties();

    void ForEachEdge(const std::function<void(Edge&)>& f);

    void ForEachBackwardEdge(const std::function<void(Edge&)>& f);

    Edge& FindEdge(const std::function<bool(const Edge&)>&);

    Edge& FindBackwardEdge(const std::function<bool(const Edge&)>& f);

    Edge& FindEdge(EdgeRange& edges, bool forward_edges, const std::function<bool(const Edge&)>& f);

    double GetCostMaxValue();

    bool operator==(const BasicVertex& other) const;

    bool operator!=(const BasicVertex& other) const;

protected:
    /**
     * Unique id.
     */
    unsigned_id_type osm_id_;

    /**
     * Edges to all neighbours.
     */
    EdgeRange edges_;
};

template <typename Edge, typename EdgeRange>
BasicVertex<Edge, EdgeRange>::BasicVertex() : osm_id_(), edges_() {}

template <typename Edge, typename EdgeRange>
BasicVertex<Edge, EdgeRange>::BasicVertex(unsigned_id_type osm_id, EdgeRange&& edges)
        : osm_id_(osm_id), edges_(std::move(edges)) {}


template <typename Edge, typename EdgeRange>
unsigned_id_type BasicVertex<Edge, EdgeRange>::GetPreviousDefaultValue() const {
    return std::numeric_limits<unsigned_id_type>::max();
}

template <typename Edge, typename EdgeRange>
void BasicVertex<Edge, EdgeRange>::ForEachEdge(const std::function<void(Edge&)>& f) {
    for (auto&& e : edges_) {
        if (e.IsForward() || e.IsTwoways()) {
            f(e);
        }
    }
}

template <typename Edge, typename EdgeRange>
void BasicVertex<Edge, EdgeRange>::ForEachBackwardEdge(const std::function<void(Edge&)>& f) {
     for (auto&& e : edges_) {
        if (e.IsBackward() || e.IsTwoways()) {
            f(e);
        }
    }
}

template <typename Edge, typename EdgeRange>
Edge& BasicVertex<Edge, EdgeRange>::FindEdge(const std::function<bool(const Edge&)>& f) {
    return FindEdge(edges_, true, f);
}


template <typename Edge, typename EdgeRange>
inline Edge& BasicVertex<Edge, EdgeRange>::FindBackwardEdge(const std::function<bool(const Edge&)>& f) {
    return FindEdge(edges_, false, f);
}

template <typename Edge, typename EdgeRange>
Edge& BasicVertex<Edge, EdgeRange>::FindEdge(EdgeRange& edges, bool forward_edges, const std::function<bool(const Edge&)>& f) {
    double min_length = std::numeric_limits<double>::max();
    Edge* edge = nullptr;
    for (auto&& e : edges) {
        if (forward_edges && e.IsBackward()) {
            continue;
        } else if (e.IsForward()) {
            continue;
        }
        if (f(e) && e.get_length() < min_length) {
            edge = &e;
        }
    }
    if (edge) {
        return *edge;
    } else {
        throw EdgeNotFoundException("Edge not found");
    }
}

template <typename Edge, typename EdgeRange>
inline bool BasicVertex<Edge, EdgeRange>::operator==(const BasicVertex<Edge, EdgeRange>& other) const {
    return osm_id_ == other.osm_id_;
}

template <typename Edge, typename EdgeRange>
inline bool BasicVertex<Edge, EdgeRange>::operator!=(const BasicVertex<Edge, EdgeRange>& other) const {
    return !((*this) == other);
}


}

#endif //BACKEND_BASIC_VERTEX_H