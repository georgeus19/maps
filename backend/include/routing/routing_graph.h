#ifndef ROUTING_QUERY_ROUTING_GRAPH_H
#define ROUTING_QUERY_ROUTING_GRAPH_H

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
 * RoutingGraph is built on top of any graph.
 * It provides an interface which allows adding vertices and edges to the underlying graph 
 * and simulates the behaviour as if they were adde to the graph.
 * However, they are instead stored in this instance and the underlying graph is uncahnged.
 * 
 * This is useful for adding temporary vertices and their edges.
 */
template <typename Graph>
class RoutingGraph {
public:
    using Vertex = typename Graph::Vertex;
    using Edge = typename Graph::Edge;

    RoutingGraph(Graph& g);

    void AddVertex(Vertex&& vertex);

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
        if (vertex.get_uid() == id) {
            return vertex;
        }
    }
    return g_.GetVertex(id);
}





}
#endif //ROUTING_QUERY_ROUTING_GRAPH_H