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
    inline unsigned_id_type get_uid() const {
        return uid_;
    } 

    inline void set_uid(unsigned_id_type osm_id) {
        uid_ = osm_id;
    } 

    inline EdgeRange& get_edges() {
        return edges_;
    }

    inline void set_edges(EdgeRange&& edges) {
        edges_ = std::move(edges);
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
    unsigned_id_type uid_;

    /**
     * Edges to all neighbours.
     */
    EdgeRange edges_;
};

template <typename Edge, typename EdgeRange>
BasicVertex<Edge, EdgeRange>::BasicVertex() : uid_(0), edges_() {}

template <typename Edge, typename EdgeRange>
BasicVertex<Edge, EdgeRange>::BasicVertex(unsigned_id_type osm_id, EdgeRange&& edges)
        : uid_(osm_id), edges_(std::move(edges)) {}


template <typename Edge, typename EdgeRange>
unsigned_id_type BasicVertex<Edge, EdgeRange>::GetPreviousDefaultValue() const {
    return std::numeric_limits<unsigned_id_type>::max();
}

template <typename Edge, typename EdgeRange>
void BasicVertex<Edge, EdgeRange>::ForEachEdge(const std::function<void(Edge&)>& f) {
    for (auto&& e : edges_) {
        if (e.IsForward() || e.IsTwoway()) {
            f(e);
        }
    }
}

template <typename Edge, typename EdgeRange>
void BasicVertex<Edge, EdgeRange>::ForEachBackwardEdge(const std::function<void(Edge&)>& f) {
     for (auto&& e : edges_) {
        if (e.IsBackward() || e.IsTwoway()) {
            f(e);
        }
    }
}

template <typename Edge, typename EdgeRange>
Edge& BasicVertex<Edge, EdgeRange>::FindEdge(const std::function<bool(const Edge&)>& f) {
    double min_length = std::numeric_limits<double>::max();
    Edge* edge = nullptr;
    ForEachEdge([&](Edge& e){
        if (f(e) && e.get_length() < min_length) {
            min_length = e.get_length();
            edge = &e;
        }
    });
    if (edge) {
        return *edge;
    } else {
        throw EdgeNotFoundException("Forward edge not found ");
    }
}

template <typename Edge, typename EdgeRange>
inline Edge& BasicVertex<Edge, EdgeRange>::FindBackwardEdge(const std::function<bool(const Edge&)>& f) {
    double min_length = std::numeric_limits<double>::max();
    Edge* edge = nullptr;
    ForEachBackwardEdge([&](Edge& e){
        if (f(e) && e.get_length() < min_length) {
            min_length = e.get_length();
            edge = &e;
        }
    });
    if (edge) {
        return *edge;
    } else {
        throw EdgeNotFoundException("Backward edge not found ");
    }
}

template <typename Edge, typename EdgeRange>
inline bool BasicVertex<Edge, EdgeRange>::operator==(const BasicVertex<Edge, EdgeRange>& other) const {
    return uid_ == other.uid_;
}

template <typename Edge, typename EdgeRange>
inline bool BasicVertex<Edge, EdgeRange>::operator!=(const BasicVertex<Edge, EdgeRange>& other) const {
    return !((*this) == other);
}


}

#endif //BACKEND_BASIC_VERTEX_H