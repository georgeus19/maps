#ifndef ROUTING_EDGE_RANGES_ITERATOR_EDGE_RANGE_H
#define ROUTING_EDGE_RANGES_ITERATOR_EDGE_RANGE_H

namespace routing {

template <typename Edge, typename EdgeIterator>
class IteratorEdgeRange {
public:

    IteratorEdgeRange();

    IteratorEdgeRange(EdgeIterator begin, EdgeIterator end);

    EdgeIterator begin();

    EdgeIterator end();

private:
    EdgeIterator begin_;
    EdgeIterator end_;
};

template <typename Edge, typename EdgeIterator>
inline IteratorEdgeRange<Edge, EdgeIterator>::IteratorEdgeRange() : begin_(), end_() {}

template <typename Edge, typename EdgeIterator>
inline IteratorEdgeRange<Edge, EdgeIterator>::IteratorEdgeRange(EdgeIterator begin, EdgeIterator end) : begin_(begin), end_(end) {}

template <typename Edge, typename EdgeIterator>
inline EdgeIterator IteratorEdgeRange<Edge, EdgeIterator>::begin() {
    return begin_;
}

template <typename Edge, typename EdgeIterator>
inline EdgeIterator IteratorEdgeRange<Edge, EdgeIterator>::end() {
    return end_;
}


}
#endif //ROUTING_EDGE_RANGES_ITERATOR_EDGE_RANGE_H
