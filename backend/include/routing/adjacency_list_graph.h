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
#include <algorithm>
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

    void AddEdge(E&& edge, unsigned_id_type target_vertex_id, const std::function<void(V&, E&& e)>& add_edge);

    size_t GetVertexCount() const;

    size_t GetEdgeCount();

private:
    /**
     * Internal graph representations. Unfortunately, even though the ids
     * are number they are too high to index using std::vector.
     */
    std::vector<V> vertices_;

    void SetOsmId(unsigned_id_type vertex_id);

};

template <typename V, typename E>
AdjacencyListGraph<V, E>::AdjacencyListGraph() : vertices_() {}

template <typename V, typename E>
inline void AdjacencyListGraph<V, E>::AddEdge(E&& edge) {
    unsigned_id_type from_node = edge.get_from();
    AddEdge(std::move(edge), from_node, [](V& v, E&& e) {
        v.get_edges().AddEdge(std::move(e));            
    });
}

template <typename V, typename E>
inline V& AdjacencyListGraph<V, E>::GetVertex(unsigned_id_type id) {
    assert(id < vertices_.size());
    return vertices_[id];
}

template <typename V, typename E>
void AdjacencyListGraph<V, E>::ForEachVertex(const std::function<void(V&)>& f) {
    for (auto&& vertex : vertices_) {
        if (vertex.get_osm_id() != 0) {
            f(vertex);
        }
    }
}

template <typename V, typename E>
void AdjacencyListGraph<V, E>::ForEachEdge(const std::function<void(E&)>& f) {
    ForEachVertex([&](V& vertex) {
        vertex.ForEachEdge(f);
    });
}

template <typename V, typename E>
void AdjacencyListGraph<V, E>::AddEdge(E&& e, unsigned_id_type target_vertex_id, const std::function<void(V&, E&& e)>& add_edge) {
    unsigned_id_type max_vertex_id = std::max(e.get_from(), e.get_to());
    if (vertices_.size() <= max_vertex_id) {
        vertices_.resize(max_vertex_id + 1);
    }
    assert(target_vertex_id < vertices_.size());
    SetOsmId(e.get_from());
    SetOsmId(e.get_to());
    auto&& vertex = GetVertex(target_vertex_id);
    add_edge(vertex, std::move(e));
}

template <typename V, typename E>
size_t AdjacencyListGraph<V, E>::GetVertexCount() const {
    return vertices_.size();
}

template <typename V, typename E>
size_t AdjacencyListGraph<V, E>::GetEdgeCount() {
    size_t count = 0;
    ForEachEdge([&](E&){
        ++count;
    });
    return count;
}

template <typename V, typename E>
void AdjacencyListGraph<V, E>::SetOsmId(unsigned_id_type vertex_id) {
    auto&& vertex = GetVertex(vertex_id);
    vertex.set_osm_id(vertex_id);
}


}

#endif //BACKEND_ROUTING_ADJACENCY_GRAPH_H