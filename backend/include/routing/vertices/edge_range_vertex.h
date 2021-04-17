#ifndef BACKEND_EDGE_RANGE_VERTEX_H
#define BACKEND_EDGE_RANGE_VERTEX_H
#include "routing/edges/basic_edge.h"
#include <vector>
#include <limits>
#include <functional>
#include "routing/exception.h"

namespace routing {

/**
 * EdgeRangeVertex is a routing graph vertex with only the most general properties.
 */
template <typename Edge, typename EdgeIterator>
class EdgeRangeVertex {
protected:
    class EdgeRange;
public:
    inline unsigned_id_type get_osm_id() const {
        return osm_id_;
    } 

    inline EdgeRange& get_edges() {
        return edges_;
    }

    EdgeRangeVertex();

    EdgeRangeVertex(unsigned_id_type osm_id, EdgeIterator begin, EdgeIterator end);

    EdgeRangeVertex(const EdgeRangeVertex<Edge, EdgeIterator> & other) = default;

    EdgeRangeVertex(EdgeRangeVertex<Edge, EdgeIterator> && other) = default;

    EdgeRangeVertex& operator=(const EdgeRangeVertex & other) = default;
    EdgeRangeVertex& operator=(EdgeRangeVertex && other) = default;

    ~EdgeRangeVertex() = default;

    void ForEachEdge(const std::function<void(Edge&)>&);

    Edge& FindEdge(const std::function<bool(const Edge&)>&);

    double GetCostMaxValue();

    bool operator==(const EdgeRangeVertex& other) const;

    bool operator!=(const EdgeRangeVertex& other) const;
protected:
    /**
     * Unique id.
     */
    unsigned_id_type osm_id_;

    EdgeRange edges_;


    class EdgeRange {
    public:

        EdgeRange(EdgeIterator begin, EdgeIterator end) : begin_(begin), end_(end) {}

        EdgeRange() : begin_(), end_() {}

        EdgeIterator begin() {
            return begin_;
        }

        EdgeIterator end() {
            return end_;
        }
    private:
        EdgeIterator begin_;
        EdgeIterator end_;

    };

    Edge& FindEdge(EdgeRange& edges, const std::function<bool(const Edge&)>& f);
};

template <typename Edge, typename EdgeIterator>
EdgeRangeVertex<Edge, EdgeIterator>::EdgeRangeVertex()
        : osm_id_(), edges_() {}

template <typename Edge, typename EdgeIterator>
EdgeRangeVertex<Edge, EdgeIterator>::EdgeRangeVertex(unsigned_id_type osm_id, EdgeIterator begin, EdgeIterator end)
        : osm_id_(osm_id), edges_(begin, end) {}


template <typename Edge, typename EdgeIterator>
void EdgeRangeVertex<Edge, EdgeIterator>::ForEachEdge(const std::function<void(Edge&)>& f) {
    std::for_each(edges_.begin(), edges_.end(), f);
}

template <typename Edge, typename EdgeIterator>
inline Edge& EdgeRangeVertex<Edge, EdgeIterator>::FindEdge(const std::function<bool(const Edge&)>& f) {
    return FindEdge(edges_, f);
}

template <typename Edge, typename EdgeIterator>
inline bool EdgeRangeVertex<Edge, EdgeIterator>::operator==(const EdgeRangeVertex<Edge, EdgeIterator>& other) const {
    return osm_id_ == other.osm_id_;
}

template <typename Edge, typename EdgeIterator>
inline bool EdgeRangeVertex<Edge, EdgeIterator>::operator!=(const EdgeRangeVertex<Edge, EdgeIterator>& other) const {
    return !((*this) == other);
}

template <typename Edge, typename EdgeIterator>
Edge& EdgeRangeVertex<Edge, EdgeIterator>::FindEdge(EdgeRange& edges, const std::function<bool(const Edge&)>& f) {
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



}

#endif //BACKEND_EDGE_RANGE_VERTEX_H