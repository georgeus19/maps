#ifndef BACKEND_ROUTING_CH_SEARCH_GRAPH_H
#define BACKEND_ROUTING_CH_SEARCH_GRAPH_H
#include <unordered_map>
#include <vector>
#include <utility>
#include <functional>
#include <queue>
#include <string>

#include "routing/edges/basic_edge.h"
#include "database/database_helper.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/vertices/contraction_vertex.h"
#include "routing/exception.h"
#include <set>
#include <vector>

namespace routing {

/**
 * Routing graph which can be used for any edge and vertex types.
 * However, Vertex and Edge must be valid with respect to each other.
 * @tparam Vertex Type of vertex in the graph.
 * @tparam Edge Type of edge in the graph.
 */
template <typename V, typename E>
class CHSearchGraph {
public:
    using Vertex = V;
    using Edge = E;
    using EdgeIterator = std::vector<E>::iterator;

    CHSearchGraph();

    template <typename Graph>
    void Load(Graph& graph);
    /**
     * Return point to vertex with `id`.
     * @param id Id of vertex to which Vertex* points.
     * @return Pointer to vetex or nullptr if it is not found.
     */
    V& GetVertex(unsigned_id_type id);

    void ForEachVertex(const std::function<void(V&)>& f);

    void ForEachEdge(const std::function<void(E&)>& f);

    size_t GetVertexCount() const;

    size_t GetEdgeCount();

private:

    std::vector<V> vertices_;

    std::vector<E> edges_;

    struct Capacities {
        size_t vertices_capacity;
        size_t edges_capacity;

        Capacities(size_t vc, size_t ec) : vertices_capacity(vc), edges_capacity(ec) {}
    };

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
    assert(id == vertices_[id].get_osm_id());
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
        // vertex.ForEachEdge(f);
        // vertex.ForEachBackwardEdge(f);
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
template <typename Graph>
CHSearchGraph<V, E>::Capacities CHSearchGraph<V, E>::PrecomputeCapacities(Graph& graph) {
    size_t max_vertex_id = 0;
    size_t edges = 0;
    graph.ForEachVertex([&](typename Graph::Vertex& vertex) {
        for(auto&& edge : vertex.get_edges()) {
            if (IsToHigherOrderingRank(graph, edge)) {
                ++edges;
            }
            // if (edge.IsTwoway()) {
            //     ++edges;
            // } else if (IsToHigherOrderingRank(graph, edge)) {
            //     ++edges;
            // }
        }
        if (vertex.get_osm_id() > max_vertex_id) {
            max_vertex_id = vertex.get_osm_id();
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
            edges_begin_it = edges_end_it;
            if (IsToHigherOrderingRank(graph, edge)) {
                edges_.push_back(edge);
                ++edges_end_it;
            }
            // bool add = false;
            // if (edge.IsTwoway()) {
            //     add = true;
            // } else {
            //     add = IsToHigherOrderingRank(graph, edge);
            // }
            // if (add) {
            //     edges_.push_back(edge);
            //     ++edges_end_it;
            // }
        }
        vertices_[vertex.get_osm_id()] = V{vertex.get_osm_id(), typename V::EdgeRange{edges_begin_it, edges_end_it}, vertex.get_ordering_rank()};
    });
}

template <typename V, typename E>
template <typename Graph>
bool CHSearchGraph<V, E>::IsToHigherOrderingRank(Graph& graph, const typename Graph::Edge& edge) {
    unsigned_id_type from, to;
    if (edge.IsForward() || edge.IsTwoway()) {
        from = edge.get_from();
        to = edge.get_to();
    }
    if (edge.IsBackward()) {
        from = edge.get_backward_from();
        to = edge.get_backward_to();
    }
    if (graph.GetVertex(from).get_ordering_rank() < graph.GetVertex(to).get_ordering_rank()) {
        return true;
    }
    return false;
}



}

#endif //BACKEND_ROUTING_CH_SEARCH_GRAPH_H