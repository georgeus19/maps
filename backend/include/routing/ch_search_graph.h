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

    std::vector<E> upward_edges_;

    std::vector<E> downward_edges_;

    struct Capacities {
        size_t vertices_capacity;
        size_t upward_edges_capacity;
        size_t downward_edges_capacity;

        Capacities(size_t vc, size_t uec, size_t dec) : vertices_capacity(vc), upward_edges_capacity(uec), downward_edges_capacity(dec) {}
    };

    template <typename Graph>
    Capacities PrecomputeCapacities(Graph& graph);

    template <typename Graph>
    void LoadEdges(Graph& graph);

    template <typename Graph>
    void LoadEdge(Graph& graph, const typename Graph::Vertex& from, const typename Graph::Edge& edge,
        std::vector<E>& output_edges, typename std::vector<E>::iterator& end_it);
};

template <typename V, typename E>
CHSearchGraph<V, E>::CHSearchGraph() : vertices_(), upward_edges_(), downward_edges_() {}

template <typename V, typename E>
template <typename Graph>
void CHSearchGraph<V, E>::Load(Graph& graph) {
    Capacities capacities = PrecomputeCapacities(graph);
    vertices_.assign(capacities.vertices_capacity, V{});
    upward_edges_.reserve(capacities.upward_edges_capacity);
    downward_edges_.reserve(capacities.downward_edges_capacity);
    LoadEdges(graph);
    std::cout << "old upward_edges_.capacity: " << capacities.upward_edges_capacity << ::std::endl;
    std::cout << "old downward_edges_.capacity: " << capacities.downward_edges_capacity << ::std::endl;
    std::cout << "upward_edges_.capacity: " << upward_edges_.capacity() << ::std::endl;
    std::cout << "downward_edges_.capacity: " << downward_edges_.capacity() << ::std::endl;
    std::cout << "upward_edges_.size: " << upward_edges_.size() << ::std::endl;
    std::cout << "downward_edges_.size: " << downward_edges_.size() << ::std::endl;
    assert(capacities.upward_edges_capacity == upward_edges_.capacity());
    assert(capacities.downward_edges_capacity == downward_edges_.capacity());
}

template <typename V, typename E>
inline V& CHSearchGraph<V, E>::GetVertex(unsigned_id_type id) {
    assert(id > 0 && id < vertices_.size());
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
    for(auto&& edge : upward_edges_) {
        f(edge);
    }

    for(auto&& reverse_edge : downward_edges_) {
        f(reverse_edge);
    }
}

template <typename V, typename E>
size_t CHSearchGraph<V, E>::GetVertexCount() const {
    return vertices_.size();
}

template <typename V, typename E>
size_t CHSearchGraph<V, E>::GetEdgeCount() {
    return upward_edges_.size() + downward_edges_.size();
}

template <typename V, typename E>
template <typename Graph>
CHSearchGraph<V, E>::Capacities CHSearchGraph<V, E>::PrecomputeCapacities(Graph& graph) {
    size_t vertex_count = graph.GetVertexCount() + 1;
    size_t upward_edges = 0;
    size_t downward_edges = 0;
    graph.ForEachVertex([&](typename Graph::Vertex& vertex) {
        for(auto&& edge : vertex.get_edges()) {
            auto&& to = graph.GetVertex(edge.get_to());
            if (vertex.get_ordering_rank() < to.get_ordering_rank()) {
                ++upward_edges;
            } else {
                ++downward_edges;
            }
        }
    });
    assert(upward_edges + downward_edges == graph.GetEdgeCount());
    return Capacities{vertex_count, upward_edges, downward_edges};
}

template <typename V, typename E>
template <typename Graph>
void CHSearchGraph<V, E>::LoadEdges(Graph& graph) {
    auto upward_edges_begin_it = upward_edges_.begin();
    auto upward_edges_end_it = upward_edges_.begin();
    auto downward_edges_begin_it = downward_edges_.begin();
    auto downward_edges_end_it = downward_edges_.begin();
    graph.ForEachVertex([&](typename Graph::Vertex& vertex) { 
        upward_edges_begin_it = upward_edges_end_it;
        for(auto&& edge : vertex.get_edges()) {
            LoadEdge(graph, vertex, edge, upward_edges_, upward_edges_end_it);
        }
        downward_edges_begin_it = downward_edges_end_it;
        for(auto&& redge : vertex.get_reverse_edges()) {
            LoadEdge(graph, vertex, redge, downward_edges_, downward_edges_end_it);
        }
        vertices_[vertex.get_osm_id()] = V{vertex.get_osm_id(), vertex.get_ordering_rank(), upward_edges_begin_it, upward_edges_end_it, downward_edges_begin_it, downward_edges_end_it};
    });
}

template <typename V, typename E>
template <typename Graph>
void CHSearchGraph<V, E>::LoadEdge(Graph& graph, const typename Graph::Vertex& from, const typename Graph::Edge& edge,
    std::vector<E>& output_edges, typename std::vector<E>::iterator& end_it) {
    auto&& to = graph.GetVertex(edge.get_to());
    if (from.get_ordering_rank() < to.get_ordering_rank()) {
        output_edges.emplace_back(edge.get_uid(), edge.get_from(), edge.get_to(), edge.get_length(), edge.get_contracted_vertex());
        ++end_it;
    }
}



}

#endif //BACKEND_ROUTING_CH_SEARCH_GRAPH_H