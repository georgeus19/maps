#ifndef BACKEND_ROUTING_ADJACENCY_GRAPH_H
#define BACKEND_ROUTING_ADJACENCY_GRAPH_H
#include "routing/edges/basic_edge.h"
#include "database/database_helper.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/vertices/contraction_vertex.h"
#include "routing/exception.h"

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
 * Routing graph which can be used for any edge and vertex types.
 * However, Vertex and Edge must be valid with respect to each other.
 * @tparam Vertex Type of vertex in the graph.
 * @tparam Edge Type of edge in the graph.
 */
template <typename V, typename E>
class AdjacencyListGraph {
public:
    using Vertex = V;
    using Edge = E;

    AdjacencyListGraph();

    /**
     * Add `edge` to graph.
     * @param edge Edge which is added to graph.
     */
    void AddEdge(E&& edge);

    /**
     * Return point to vertex with `id`.
     * @param id Id of vertex to which Vertex* points.
     * @return Pointer to vetex or nullptr if it is not found.
     */
    V& GetVertex(unsigned_id_type id);

    void ForEachVertex(const std::function<void(V&)>& f);

    void ForEachEdge(const std::function<void(E&)>& f);

    void AddEdge(E&& edge, const std::function<void(V &, E&& e)>& add_edge);

    size_t GetVertexCount() const;

    size_t GetEdgeCount();

private:
    /**
     * Internal graph representaions. Unfortunately, even though the ids
     * are number they are too high to index using std::vector.
     */
    std::vector<V> g_;

};

template <typename V, typename E>
AdjacencyListGraph<V, E>::AdjacencyListGraph() : g_() {
    // g_.reserve(1879000 + 1);
    // for(uint64_t i = 0; i < 1879000 + 1; ++i) {
    //     g_.push_back(V{i});
    // }
}

template <typename V, typename E>
inline void AdjacencyListGraph<V, E>::AddEdge(E && edge) {
    AddEdge(std::move(edge), [](V& v, E&& e) {
        v.get_edges().AddEdge(std::move(e));            
    });
}

template <typename V, typename E>
inline V& AdjacencyListGraph<V, E>::GetVertex(unsigned_id_type id) {
    assert(id < g_.size());
    return g_[id];
}

template <typename V, typename E>
void AdjacencyListGraph<V, E>::ForEachVertex(const std::function<void(V&)>& f) {
    for (auto&& vertex : g_) {
        f(vertex);
    }
}

template <typename V, typename E>
void AdjacencyListGraph<V, E>::ForEachEdge(const std::function<void(E&)>& f) {
    ForEachVertex([&](V& vertex) {
        vertex.ForEachEdge(f);
    });
}

template <typename V, typename E>
void AdjacencyListGraph<V, E>::AddEdge(E && e, const std::function<void(V &, E && e)>& add_edge) {
    unsigned_id_type from_node = e.get_from();
    assert(from_node < g_.size()); 
    auto&& vertex = g_[from_node];
    add_edge(vertex, std::move(e));
}

template <typename V, typename E>
size_t AdjacencyListGraph<V, E>::GetVertexCount() const {
    return g_.size();
}

template <typename V, typename E>
size_t AdjacencyListGraph<V, E>::GetEdgeCount() {
    size_t count = 0;
    ForEachEdge([&](E&){
        ++count;
    });
    return count;
}



}

#endif //BACKEND_ROUTING_ADJACENCY_GRAPH_H