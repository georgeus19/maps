#ifndef ROUTING_CH_SEARCH_GRAPH_H
#define ROUTING_CH_SEARCH_GRAPH_H

#include "routing/edges/basic_edge.h"
#include "routing/database/database_helper.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/exception.h"
#include "routing/types.h"

#include <unordered_map>
#include <vector>
#include <utility>
#include <functional>
#include <queue>
#include <string>
#include <set>
#include <vector>

namespace routing {

/**
 * Optimized memory efficient immutable search graph for Contraction Hierarchies.
 * 
 * It contains only edges that lead to a vertex with higher ordering rank.
 */
template <typename V, typename E>
class CHSearchGraph {
public:
    using Vertex = V;
    using Edge = E;
    using EdgeIterator = typename std::vector<E>::iterator;

    CHSearchGraph();

    /**
     * Copy vertices and edges from a graph to this graph. Only edges leading to vertices with higher
     * ordering rank are copied. It is required that the graph ahs the same type of edge and a vertex
     * that contain its ordering rank.
     */
    template <typename Graph>
    void Load(Graph& graph);

    V& GetVertex(unsigned_id_type id);

    void ForEachVertex(const std::function<void(V&)>& f);

    void ForEachEdge(const std::function<void(E&)>& f);

    size_t GetVertexCount() const;

    size_t GetEdgeCount();

    unsigned_id_type GetMaxVertexId();
    
    unsigned_id_type GetMaxEdgeId();

private:

    std::vector<V> vertices_;

    std::vector<E> edges_;

    struct Capacities {
        size_t vertices_capacity;
        size_t edges_capacity;

        Capacities(size_t vc, size_t ec) : vertices_capacity(vc), edges_capacity(ec) {}
    };

    /**
     * Compute how many vertices and edges will be in the final graph.
     */
    template <typename Graph>
    Capacities PrecomputeCapacities(Graph& graph);

    template <typename Graph>
    void LoadEdges(Graph& graph);

    template <typename Graph>
    bool IsToHigherOrderingRank(Graph& graph, const typename Graph::Edge& edge);

};

template <typename V, typename E>
CHSearchGraph<V, E>::CHSearchGraph() : vertices_() {}

template <typename V, typename E>
template <typename Graph>
void CHSearchGraph<V, E>::Load(Graph& graph) {
    Capacities capacities = PrecomputeCapacities(graph);
    vertices_.assign(capacities.vertices_capacity, V{});
    edges_.reserve(capacities.edges_capacity);
    LoadEdges(graph);
    std::cout << "vertices_.size: " << vertices_.size() << ::std::endl;
    std::cout << "old edges_.capacity: " << capacities.edges_capacity << ::std::endl;
    std::cout << "edges_.capacity: " << edges_.capacity() << ::std::endl;
    std::cout << "edges_.size: " << edges_.size() << ::std::endl;
    assert(capacities.edges_capacity == edges_.capacity());
}

template <typename V, typename E>
inline V& CHSearchGraph<V, E>::GetVertex(unsigned_id_type id) {
    assert(id < vertices_.size());
    assert(id == vertices_[id].get_uid());
    return vertices_[id];
}

template <typename V, typename E>
void CHSearchGraph<V, E>::ForEachVertex(const std::function<void(V&)>& f) {
    for (auto&& vertex : vertices_) {
        f(vertex);
    }
}

template <typename V, typename E>
void CHSearchGraph<V, E>::ForEachEdge(const std::function<void(E&)>& f) {
    for (auto&& edge : edges_) {
        f(edge);
    }
}

template <typename V, typename E>
size_t CHSearchGraph<V, E>::GetVertexCount() const {
    return vertices_.size();
}

template <typename V, typename E>
size_t CHSearchGraph<V, E>::GetEdgeCount() {
    size_t count = 0;
    ForEachEdge([&](E& e) {
        ++count;
    });
    return count;
}

template <typename V, typename E>
unsigned_id_type CHSearchGraph<V, E>::GetMaxVertexId() {
    return vertices_.size();
}
    
template <typename V, typename E>
unsigned_id_type CHSearchGraph<V, E>::GetMaxEdgeId() {
    size_t max_id = 0;
    ForEachEdge([&](E& e) {
        if (e.get_uid() > max_id) {
            max_id = e.get_uid();
        }
    });
    return max_id;
}

template <typename V, typename E>
template <typename Graph>
typename CHSearchGraph<V, E>::Capacities CHSearchGraph<V, E>::PrecomputeCapacities(Graph& graph) {
    size_t max_vertex_id = 0;
    size_t edges = 0;
    graph.ForEachVertex([&](typename Graph::Vertex& vertex) {
        for(auto&& edge : vertex.get_edges()) {
            if (IsToHigherOrderingRank(graph, edge)) {
                ++edges;
            }
        }
        if (vertex.get_uid() > max_vertex_id) {
            max_vertex_id = vertex.get_uid();
        }
    });
    return Capacities{max_vertex_id + 1, edges};
}

template <typename V, typename E>
template <typename Graph>
void CHSearchGraph<V, E>::LoadEdges(Graph& graph) {
    auto edges_begin_it = edges_.begin();
    auto edges_end_it = edges_.end();
    graph.ForEachVertex([&](typename Graph::Vertex& vertex) { 
        edges_begin_it = edges_end_it;
        for(auto&& edge : vertex.get_edges()) {
            if (IsToHigherOrderingRank(graph, edge)) {
                edges_.push_back(edge);
                ++edges_end_it;
            }
        }
        vertices_[vertex.get_uid()] = V{vertex.get_uid(), typename V::EdgeRange{edges_begin_it, edges_end_it}, vertex.get_ordering_rank()};
    });
}

template <typename V, typename E>
template <typename Graph>
bool CHSearchGraph<V, E>::IsToHigherOrderingRank(Graph& graph, const typename Graph::Edge& edge) {
    unsigned_id_type from, to;
    from = edge.get_from();
    to = edge.get_to();
    if (graph.GetVertex(from).get_ordering_rank() < graph.GetVertex(to).get_ordering_rank()) {
        return true;
    }
    return false;
}



}

#endif //ROUTING_CH_SEARCH_GRAPH_H