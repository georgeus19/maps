#ifndef BACKEND_BIDIRECTIONAL_GRAPH_H
#define BACKEND_BIDIRECTIONAL_GRAPH_H
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
#include "routing/graph.h"
#include <set>

namespace routing {

/**
 * Routing graph which can be used for any edge and vertex types.
 * However, Vertex and Edge must be valid with respect to each other.
 * @tparam Vertex Type of vertex in the graph.
 * @tparam Edge Type of edge in the graph.
 */
template <typename Vertex, typename Edge>
class BidirectionalGraph {
public:
    using vertex_iterator = std::unordered_map<unsigned_id_type, Vertex>::iterator;

    using V = Vertex;
    using E = Edge;

    inline BidirectionalGraph() : g_() {}

    inline void AddEdge(Edge&& edge) {
        Edge reverse_edge = edge;
        g_.AddEdge(std::move(edge));
        g_.AddReverseEdge(std::move(reverse_edge));
    }

    inline void AddUniqueEdge(Edge&& edge) {
        Edge reverse_edge = edge;
        g_.AddUniqueEdge(std::move(edge));
        g_.AddUniqueReverseEdge(std::move(reverse_edge));
    }

    inline Vertex& GetVertex(unsigned_id_type id) {
        return g_.GetVertex(id);
    }

    inline void ForEachVertex(const std::function<void(Vertex&)>& f) {
        g_.ForEachVertex(f);
    }

    inline void ForEachEdge(const std::function<void(Edge&)>& f) {
        g_.ForEachEdge(f);
    }

    size_t GetVertexCount() const {
        return g_.GetVertexCount();
    }

    size_t GetEdgeCount() {
        return g_.GetEdgeCount();
    }

private:
    Graph<Vertex, Edge> g_;
};


}

#endif //BACKEND_BIDIRECTIONAL_GRAPH_H