#ifndef BACKEND_CH_SEARCH_VERTEX_H
#define BACKEND_CH_SEARCH_VERTEX_H
#include "routing/edges/basic_edge.h"
#include "routing/vertices/edge_range_vertex.h"
#include <vector>
#include <limits>
#include <functional>
#include "routing/exception.h"

namespace routing {

/**
 * CHSearchVertex is a routing graph vertex with only the most general properties.
 */
template <typename Edge, typename EdgeIterator>
class CHSearchVertex : public EdgeRangeVertex<Edge, EdgeIterator> {
    using typename EdgeRangeVertex<Edge, EdgeIterator>::EdgeRange;
public:
    using EdgeRangeVertex<Edge, EdgeIterator>::FindEdge;

    inline unsigned_id_type get_ordering_rank() const {
        return ordering_rank_;
    }

    inline EdgeRange& get_reverse_edges() {
        return reverse_edges_;
    } 

    CHSearchVertex();

    CHSearchVertex(unsigned_id_type osm_id, unsigned_id_type ordering_rank, EdgeIterator edges_begin, EdgeIterator edges_end,
        EdgeIterator reverse_edges_begin, EdgeIterator reverse_edges_end);

    CHSearchVertex(const CHSearchVertex<Edge, EdgeIterator> & other) = default;
    CHSearchVertex(CHSearchVertex<Edge, EdgeIterator> && other) = default;
    CHSearchVertex& operator=(const CHSearchVertex & other) = default;
    CHSearchVertex& operator=(CHSearchVertex && other) = default;
    ~CHSearchVertex() = default;

    void ForEachReverseEdge(const std::function<void(Edge&)>& f);

    Edge& FindReverseEdge(const std::function<bool(const Edge&)>& f);

protected:
    unsigned_id_type ordering_rank_;

    EdgeRange reverse_edges_;
};
template <typename Edge, typename EdgeIterator>
CHSearchVertex<Edge, EdgeIterator>::CHSearchVertex() : EdgeRangeVertex<Edge, EdgeIterator>(), ordering_rank_(), reverse_edges_() {}

template <typename Edge, typename EdgeIterator>
CHSearchVertex<Edge, EdgeIterator>::CHSearchVertex(unsigned_id_type osm_id, unsigned_id_type ordering_rank, EdgeIterator edges_begin, EdgeIterator edges_end,
        EdgeIterator reverse_edges_begin, EdgeIterator reverse_edges_end)
        : EdgeRangeVertex<Edge, EdgeIterator>(osm_id, edges_begin, edges_end), ordering_rank_(ordering_rank),
            reverse_edges_(reverse_edges_begin, reverse_edges_end) {}

template <typename Edge, typename EdgeIterator>
void CHSearchVertex<Edge, EdgeIterator>::ForEachReverseEdge(const std::function<void(Edge&)>& f) {
    std::for_each(reverse_edges_.begin(), reverse_edges_.end(), f);
}

template <typename Edge, typename EdgeIterator>
inline Edge& CHSearchVertex<Edge, EdgeIterator>::FindReverseEdge(const std::function<bool(const Edge&)>& f) {
    return FindEdge(reverse_edges_, f);
}




}

#endif //BACKEND_CH_SEARCH_VERTEX_H