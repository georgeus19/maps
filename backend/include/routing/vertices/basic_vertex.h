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

    /**
     * How much it costs to go to this vertex from the start point
     * of a routing algorithm.
     */
    double cost_;

    /**
     * Id of vertex that is one edge closer to the start
     * point of the best route.
     */
    unsigned_id_type previous_;

public:
    inline unsigned_id_type get_osm_id() const {
        return osm_id_;
    } 

    inline double get_cost() const {
        return cost_;
    }

    inline void set_cost(double cost) {
        cost_ = cost;
    }

    inline std::vector<Edge>& get_edges() {
        return outgoing_edges_;
    }

    inline unsigned_id_type get_previous() const {
        return previous_;
    }

    inline void set_previous(unsigned_id_type previous) {
        previous_ = previous;
    }

    BasicVertex(unsigned_id_type osm_id);

    BasicVertex(const BasicVertex<Edge> & other);

    BasicVertex(BasicVertex<Edge> && other);

    BasicVertex& operator=(const BasicVertex & other);

    BasicVertex& operator=(BasicVertex && other);

    ~BasicVertex();

    void Swap(BasicVertex & other);

    void AddEdge(Edge&& edge);

    void AddEdge(const Edge & edge);

    unsigned_id_type GetPreviousDefaultValue() const;

    void ResetRoutingProperties();

    void ForEachEdge(const std::function<void(Edge&)>&);

    Edge& FindEdge(const std::function<bool(const Edge&)>&);

    Edge& FindEdge(std::vector<Edge>& edges, const std::function<bool(const Edge&)>& f);

    double GetCostMaxValue();
};

template <typename Edge>
BasicVertex<Edge>::BasicVertex(unsigned_id_type osm_id)
        : osm_id_(osm_id), outgoing_edges_(), cost_(std::numeric_limits<double>::max()), previous_(GetPreviousDefaultValue()) {}

template <typename Edge>
BasicVertex<Edge>::BasicVertex(const BasicVertex<Edge> & other) {
    osm_id_ = other.osm_id_;
    outgoing_edges_ = other.outgoing_edges_;
    cost_ = other.cost_;
    previous_ = other.previous_;
}

template <typename Edge>
BasicVertex<Edge>::BasicVertex(BasicVertex<Edge> && other) {
    osm_id_ = other.osm_id_;
    outgoing_edges_ = std::move(other.outgoing_edges_);
    other.outgoing_edges_ = std::vector<Edge>{};
    cost_ = other.cost_;
    previous_ = other.previous_;
}

template <typename Edge>
BasicVertex<Edge>& BasicVertex<Edge>::operator=(const BasicVertex<Edge> & other) {
    BasicVertex tmp{other};
    Swap(tmp);
    return *this;
}

template <typename Edge>
BasicVertex<Edge>& BasicVertex<Edge>::operator=(BasicVertex<Edge> && other) {
    if (this != &other) {
        osm_id_ = other.osm_id_;
        outgoing_edges_ = std::move(other.outgoing_edges_);
        other.outgoing_edges_ = std::vector<Edge>{};
        cost_ = other.cost_;
        previous_ = other.previous_;
    }
    return *this;
}

template <typename Edge>
BasicVertex<Edge>::~BasicVertex() {}

template <typename Edge>
void BasicVertex<Edge>::Swap(BasicVertex<Edge> & other) {
    std::swap(osm_id_, other.osm_id_);
    std::swap(outgoing_edges_, other.outgoing_edges_);
    std::swap(cost_, other.cost_);
    std::swap(previous_, other.previous_);
}

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
void BasicVertex<Edge>::ResetRoutingProperties() {
    previous_ = GetPreviousDefaultValue();
    cost_ = GetCostMaxValue();
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
    double min_length = GetCostMaxValue();
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
inline double BasicVertex<Edge>::GetCostMaxValue() {
    return std::numeric_limits<double>::max();
}


}

#endif //BACKEND_BASIC_VERTEX_H