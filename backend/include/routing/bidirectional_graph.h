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
#include "routing/adjacency_list_graph.h"
#include <set>

namespace routing {

/**
 * Routing graph which can be used for any edge and vertex types.
 * However, Vertex and Edge must be valid with respect to each other.
 * @tparam Vertex Type of vertex in the graph.
 * @tparam Edge Type of edge in the graph.
 */
template <typename Graph>
class BidirectionalGraph {
public:
    using Vertex = Graph::Vertex;
    using Edge = Graph::Edge;

    inline BidirectionalGraph() : g_() {}

    inline void AddEdge(Edge&& edge) {
        Edge reverse_edge = edge;
        g_.AddEdge(std::move(edge));
        AddBackwardEdge(std::move(reverse_edge));
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
    Graph g_;

    inline void AddBackwardEdge(Edge&& edge) {
        unsigned_id_type backward_from_node = edge.get_backward_from();
        if (edge.IsForward()) {
            edge.SetBackward();
        }
        g_.AddEdge(std::move(edge), backward_from_node, [](Vertex& v, Edge&& e){
            v.get_edges().AddEdge(std::move(e));
        });
    }

};


}

#endif //BACKEND_BIDIRECTIONAL_GRAPH_H