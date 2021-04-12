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
template <typename Edge>
class BasicVertex {
protected:
    /**
     * Unique id.
     */
    unsigned_id_type osm_id_;

    /**
     * Edges which have origin in this to all neighbours.
     */
    std::vector<Edge> outgoing_edges_;

public:
    inline unsigned_id_type get_osm_id() const {
        return osm_id_;
    } 

    inline std::vector<Edge>& get_edges() {
        return outgoing_edges_;
    }

    BasicVertex(unsigned_id_type osm_id);

    BasicVertex(const BasicVertex<Edge> & other) = default;

    BasicVertex(BasicVertex<Edge> && other) = default;

    BasicVertex& operator=(const BasicVertex & other) = default;
    BasicVertex& operator=(BasicVertex && other) = default;

    ~BasicVertex() = default;

    void AddEdge(Edge&& edge);

    void AddEdge(const Edge & edge);

    unsigned_id_type GetPreviousDefaultValue() const;

    void ResetRoutingProperties();

    void ForEachEdge(const std::function<void(Edge&)>&);

    Edge& FindEdge(const std::function<bool(const Edge&)>&);

    Edge& FindEdge(std::vector<Edge>& edges, const std::function<bool(const Edge&)>& f);

    double GetCostMaxValue();

    bool operator==(const BasicVertex& other) const;

    bool operator!=(const BasicVertex& other) const;
};

template <typename Edge>
BasicVertex<Edge>::BasicVertex(unsigned_id_type osm_id)
        : osm_id_(osm_id), outgoing_edges_() {}


template <typename Edge>
void BasicVertex<Edge>::AddEdge(Edge&& edge) {
    outgoing_edges_.push_back(std::move(edge));
}

template <typename Edge>
void BasicVertex<Edge>::AddEdge(const Edge & edge) {
    outgoing_edges_.push_back(edge);
}

template <typename Edge>
unsigned_id_type BasicVertex<Edge>::GetPreviousDefaultValue() const {
    return std::numeric_limits<unsigned_id_type>::max();
}

template <typename Edge>
void BasicVertex<Edge>::ForEachEdge(const std::function<void(Edge&)>& f) {
    std::for_each(outgoing_edges_.begin(), outgoing_edges_.end(), f);
}

template <typename Edge>
inline Edge& BasicVertex<Edge>::FindEdge(const std::function<bool(const Edge&)>& f) {
    return FindEdge(outgoing_edges_, f);
}

template <typename Edge>
Edge& BasicVertex<Edge>::FindEdge(std::vector<Edge>& edges, const std::function<bool(const Edge&)>& f) {
    double min_length = std::numeric_limits<double>::max();
    Edge* edge = nullptr;
    for (auto&& e : edges) {
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

template <typename Edge>
inline bool BasicVertex<Edge>::operator==(const BasicVertex<Edge>& other) const {
    return osm_id_ == other.osm_id_;
}

template <typename Edge>
inline bool BasicVertex<Edge>::operator!=(const BasicVertex<Edge>& other) const {
    return !((*this) == other);
}


}

#endif //BACKEND_BASIC_VERTEX_H