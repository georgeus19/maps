#ifndef BACKEND_ROUTING_GRAPH_H
#define BACKEND_ROUTING_GRAPH_H
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
template <typename Graph>
class RoutingGraph {
public:
    using Vertex = typename Graph::Vertex;
    using Edge = typename Graph::Edge;

    RoutingGraph(Graph& g);

    /**
     * Add `edge` to graph.
     * @param edge Edge which is added to graph.
     */
    void AddEdge(Edge&& edge);

    /**
     * Return point to vertex with `id`.
     * @param id Id of vertex to which Vertex* points.
     * @return Pointer to vetex or nullptr if it is not found.
     */
    Vertex& GetVertex(unsigned_id_type id);

    void ForEachVertex(const std::function<void(Vertex&)>& f);

    void ForEachEdge(const std::function<void(Edge&)>& f);

    void AddEdge(Edge&& edge, const std::function<void(Vertex&, Edge&& e)>& add_edge);

    size_t GetVertexCount() const;

    size_t GetEdgeCount();

private:
    Graph& g_;
    std::vector<Vertex> additional_vertices_;
    std::vector<Edge> additional_edges_;

};

template <typename Graph>
RoutingGraph<Graph>::RoutingGraph(Graph& g) : g_(g) {}

template <typename Graph>
inline typename Graph::Vertex& RoutingGraph<Graph>::GetVertex(unsigned_id_type id) {
    return g_.GetVertex(id);
}

template <typename Graph>
void RoutingGraph<Graph>::ForEachVertex(const std::function<void(Vertex&)>& f) {
    g_.ForEachVertex(f);
}

template <typename Graph>
void RoutingGraph<Graph>::ForEachEdge(const std::function<void(Edge&)>& f) {
    g_.ForEachEdge(f);
}

template <typename Graph>
size_t RoutingGraph<Graph>::GetVertexCount() const {
    return g_.GetVertexCount();
}

template <typename Graph>
size_t RoutingGraph<Graph>::GetEdgeCount() {
    return g_.GetEdgeCount();
}



}

#endif //BACKEND_ROUTING_GRAPH_H