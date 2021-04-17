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

    inline BidirectionalGraph() : g_() {
        size_t edge_count = 10914384;

        // for(uint64_t i = 0; i < edge_count; ++i) {
        //     AddEdge(Edge{3, 42, 3, 2});
        //     // unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length, unsigned_id_type contracted_vertex
        // }
    }

    inline void AddEdge(Edge&& edge) {
        Edge reverse_edge = edge;
        g_.AddEdge(std::move(edge));
        AddReverseEdge(std::move(reverse_edge));
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

    inline void AddReverseEdge(Edge&& edge) {
        edge.Reverse();
        g_.AddEdge(std::move(edge), [](Vertex& v, Edge&& e){
            v.AddReverseEdge(std::move(e));
        });
    }

};


}

#endif //BACKEND_BIDIRECTIONAL_GRAPH_H