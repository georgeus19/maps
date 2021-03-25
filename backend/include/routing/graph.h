#ifndef BACKEND_GRAPH_H
#define BACKEND_GRAPH_H
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

namespace routing {

/**
 * Routing graph which can be used for any edge and vertex types.
 * However, Vertex and Edge must be valid with respect to each other.
 * @tparam Vertex Type of vertex in the graph.
 * @tparam Edge Type of edge in the graph.
 */
template <typename Vertex, typename Edge>
class Graph {
public:
    using vertex_iterator = std::unordered_map<unsigned_id_type, Vertex>::iterator;

    using V = Vertex;
    using E = Edge;

    Graph();

    /**
     * Add `edge` to graph.
     * @param edge Edge which is added to graph.
     */
    void AddEdge(Edge&& edge);

    void AddReverseEdge(Edge&& edge);

    /**
     * Return point to vertex with `id`.
     * @param id Id of vertex to which Vertex* points.
     * @return Pointer to vetex or nullptr if it is not found.
     */
    Vertex& GetVertex(unsigned_id_type id);

    void ForEachVertex(const std::function<void(Vertex&)>& f);

    void ForEachEdge(const std::function<void(Edge&)>& f);

private:
    /**
     * Internal graph representaions. Unfortunately, even though the ids
     * are number they are too high to index using std::vector.
     */
    std::unordered_map<unsigned_id_type, Vertex> g_;

    void AddEdge(Edge&& edge, const std::function<void(Vertex &, Edge&& e)>& add_edge);
};

template <typename Vertex, typename Edge>
Graph<Vertex, Edge>::Graph() : g_(std::unordered_map<unsigned_id_type, Vertex>{}) {}

template <typename Vertex, typename Edge>
inline void Graph<Vertex, Edge>::AddEdge(Edge && edge) {
    AddEdge(std::move(edge), [](Vertex& v, Edge&& e) {
        v.AddEdge(std::move(e));            
    });
}

template <typename Vertex, typename Edge>
inline void Graph<Vertex, Edge>::AddReverseEdge(Edge && edge) {
    edge.Reverse();
    AddEdge(std::move(edge), [](Vertex& v, Edge&& e){
        v.AddReverseEdge(std::move(e));
    });
}

template <typename Vertex, typename Edge>
void Graph<Vertex, Edge>::AddEdge(Edge && e, const std::function<void(Vertex &, Edge && e)>& add_edge) {
    unsigned_id_type from_node = e.get_from();
    unsigned_id_type to_node = e.get_to();
    auto&& from_it = g_.find(from_node);
    if (from_it == g_.end()) {
        unsigned_id_type from_node = e.get_from();
        // Vertex is not in the graph. Add Vertex with the edge.
        Vertex v{from_node};
        add_edge(v, std::move(e));
        g_.insert(std::make_pair<unsigned_id_type, Vertex>(std::move(from_node), std::move(v)));
    } else {
        // Vertex exists. Add edge.
        add_edge(from_it->second, std::move(e));
    }

    // e.to_ might not be added to the graph.
    // So try add it with no outgoing edges.
    auto && to_it = g_.find(to_node);
    if (to_it == g_.end()) {
        Vertex to_vertex{to_node};
        g_.insert(std::make_pair<unsigned_id_type, Vertex>(std::move(to_node), std::move(to_vertex)));
    }
}


template <typename Vertex, typename Edge>
inline Vertex& Graph<Vertex, Edge>::GetVertex(unsigned_id_type id) {
    auto&& it = g_.find(id);
    if (it == g_.end()) {
        throw VertexNotFoundException("Vertex " + std::to_string(id) + " not found in graph.");
    } else {
        return it->second;
    }
}

template <typename Vertex, typename Edge>
void Graph<Vertex, Edge>::ForEachVertex(const std::function<void(Vertex&)>& f) {
    for (auto&& pair : g_) {
        f(pair.second);
    }
}

template <typename Vertex, typename Edge>
void Graph<Vertex, Edge>::ForEachEdge(const std::function<void(Edge&)>& f) {
    ForEachVertex([&](Vertex& vertex) {
        vertex.ForEachEdge(f);
    });
    
}
    
}

#endif //BACKEND_GRAPH_H