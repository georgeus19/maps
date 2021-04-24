#ifndef BACKEND_ROUTING_VECTOR_EDGE_RANGE_H
#define BACKEND_ROUTING_VECTOR_EDGE_RANGE_H
#include <vector>

namespace routing {

template <typename Edge>
class VectorEdgeRange {
public:

    VectorEdgeRange();

    std::vector<Edge>::iterator begin();

    std::vector<Edge>::iterator end();

    void AddEdge(Edge&& edge);

    void AddEdge(const Edge& edge);

private:
    std::vector<Edge> edges_;
};

template <typename Edge>
inline VectorEdgeRange<Edge>::VectorEdgeRange() : edges_() {}

template <typename Edge>
inline std::vector<Edge>::iterator VectorEdgeRange<Edge>::begin() {
    return edges_.begin();
}

template <typename Edge>
inline std::vector<Edge>::iterator VectorEdgeRange<Edge>::end() {
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
#endif //BACKEND_ROUTING_VECTOR_EDGE_RANGE_H
