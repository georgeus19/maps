#ifndef ROUTING_QUERY_ROUTING_GRAPH_H
#define ROUTING_QUERY_ROUTING_GRAPH_H
#include <unordered_map>
#include <vector>
#include <utility>
#include <functional>
#include <queue>
#include <string>

#include "routing/edges/basic_edge.h"
#include "routing/database/database_helper.h"
#include "routing/vertices/basic_vertex.h"
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

    void AddVertex(Vertex&& vertex);

    /**
     * Return point to vertex with `id`.
     * @param id Id of vertex to which Vertex* points.
     * @return Pointer to vetex or nullptr if it is not found.
     */
    Vertex& GetVertex(unsigned_id_type id);

private:
    Graph& g_;
    std::vector<Vertex> additional_vertices_;
};

template <typename Graph>
RoutingGraph<Graph>::RoutingGraph(Graph& g) : g_(g) {}

template <typename Graph>
void RoutingGraph<Graph>::AddVertex(Vertex&& vertex) {
    additional_vertices_.push_back(std::move(vertex));
}

template <typename Graph>
inline typename Graph::Vertex& RoutingGraph<Graph>::GetVertex(unsigned_id_type id) {
    for(auto&& vertex : additional_vertices_) {
        if (vertex.get_osm_id() == id) {
            return vertex;
        }
    }
    return g_.GetVertex(id);
}




}

#endif //ROUTING_QUERY_ROUTING_GRAPH_H