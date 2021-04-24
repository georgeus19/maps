#ifndef BACKEND_ROUTING_ITERATOR_EDGE_RANGE_H
#define BACKEND_ROUTING_ITERATOR_EDGE_RANGE_H

namespace routing {

template <typename Edge, typename EdgeIterator>
class VectorEdgeRange {
public:
    EdgeIterator begin();

    EdgeIterator end();

    void AddEdge(Edge&& edge);

    void AddEdge(const Edge& edge);

private:
    EdgeIterator begin_;
    EdgeIterator end_;
};

template <typename Edge, typename EdgeIterator>
inline EdgeIterator VectorEdgeRange<Edge, EdgeIterator>::begin() {
    return begin_;
}

template <typename Edge, typename EdgeIterator>
inline EdgeIterator VectorEdgeRange<Edge, EdgeIterator>::end() {
    return end_;
}


}
#endif //BACKEND_ROUTING_ITERATOR_EDGE_RANGE_H
