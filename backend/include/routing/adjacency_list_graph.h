#ifndef BACKEND_ADJACENCY_LIST_GRAPH_H
#define BACKEND_ADJACENCY_LIST_GRAPH_H
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
#include "tsl/robin_map.h"

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
    template <typename Key, typename Value>
    using UnorderedMap = std::unordered_map<Key, Value>;
    using vertex_iterator = UnorderedMap<unsigned_id_type, V>::iterator;

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
    UnorderedMap<unsigned_id_type, V> g_;

};

template <typename V, typename E>
AdjacencyListGraph<V, E>::AdjacencyListGraph() : g_(UnorderedMap<unsigned_id_type, V>{}) {}

template <typename V, typename E>
inline void AdjacencyListGraph<V, E>::AddEdge(E && edge) {
    AddEdge(std::move(edge), [](V& v, E&& e) {
        v.AddEdge(std::move(e));            
    });
}

template <typename V, typename E>
inline V& AdjacencyListGraph<V, E>::GetVertex(unsigned_id_type id) {
    auto&& it = g_.find(id);
    if (it == g_.end()) {
        throw VertexNotFoundException("Vertex " + std::to_string(id) + " not found in graph.");
    } else {
        // return it.value();
        return it->second;
    }
}

template <typename V, typename E>
void AdjacencyListGraph<V, E>::ForEachVertex(const std::function<void(V&)>& f) {
    // for(auto it = g_.begin(); it != g_.end(); ++it) {
    //     f(it.value());
    // }
    for (auto&& pair : g_) {
        f(pair.second);
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
    unsigned_id_type to_node = e.get_to();
    auto&& from_it = g_.find(from_node);
    if (from_it == g_.end()) {
        unsigned_id_type from_node = e.get_from();
        // Vertex is not in the graph. Add Vertex with the edge.
        V v{from_node};
        add_edge(v, std::move(e));
        g_.insert(std::make_pair<unsigned_id_type, V>(std::move(from_node), std::move(v)));
    } else {
        // Vertex exists. Add edge.
        add_edge(from_it->second, std::move(e));
        // add_edge(from_it.value(), std::move(e));
    }

    // e.to_ might not be added to the graph.
    // So try add it with no outgoing edges.
    auto && to_it = g_.find(to_node);
    if (to_it == g_.end()) {
        V to_vertex{to_node};
        g_.insert(std::make_pair<unsigned_id_type, V>(std::move(to_node), std::move(to_vertex)));
    }
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

#endif //BACKEND_ADJACENCY_LIST_GRAPH_H