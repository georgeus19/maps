#ifndef ROUTING_EDGE_RANGES_VECTOR_EDGE_RANGE_H
#define ROUTING_EDGE_RANGES_VECTOR_EDGE_RANGE_H
#include <vector>

namespace routing {

template <typename Edge>
class VectorEdgeRange {
public:

    VectorEdgeRange();

    VectorEdgeRange(std::vector<Edge>&& e);

    typename std::vector<Edge>::iterator begin();

    typename std::vector<Edge>::iterator end();

    void AddEdge(Edge&& edge);

    void AddEdge(const Edge& edge);

private:
    std::vector<Edge> edges_;
};

template <typename Edge>
inline VectorEdgeRange<Edge>::VectorEdgeRange() : edges_() {}

template <typename Edge>
inline VectorEdgeRange<Edge>::VectorEdgeRange(std::vector<Edge>&& e) : edges_(std::move(e)) {}

template <typename Edge>
inline typename std::vector<Edge>::iterator VectorEdgeRange<Edge>::begin() {
    return edges_.begin();
}

template <typename Edge>
inline typename std::vector<Edge>::iterator VectorEdgeRange<Edge>::end() {
    return edges_.end();
}

template <typename Edge>
inline void VectorEdgeRange<Edge>::AddEdge(Edge&& edge) {
    edges_.push_back(std::move(edge));
}

template <typename Edge>
inline void VectorEdgeRange<Edge>::AddEdge(const Edge& edge) {
    edges_.push_back(edge);
}


}
#endif //ROUTING_EDGE_RANGES_VECTOR_EDGE_RANGE_H
