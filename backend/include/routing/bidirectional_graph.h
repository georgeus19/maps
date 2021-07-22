#ifndef ROUTING_BIDIRECTIONAL_GRAPH_H
#define ROUTING_BIDIRECTIONAL_GRAPH_H

#include "routing/edges/basic_edge.h"
#include "routing/database/database_helper.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/exception.h"
#include "routing/adjacency_list_graph.h"
#include "routing/types.h"

#include <unordered_map>
#include <vector>
#include <utility>
#include <functional>
#include <queue>
#include <string>
#include <set>

namespace routing {

/**
 * Bidirectional graph ensures that from one vertex its ingoing (= reversed backward) edges
 * and outgoing edge can be accessed.
 * 
 * It can be used on top of any graph.
 */
template <typename Graph>
class BidirectionalGraph {
public:
    using Vertex = typename Graph::Vertex;
    using Edge = typename Graph::Edge;

    inline BidirectionalGraph() : g_() {}

    BidirectionalGraph(const BidirectionalGraph& other) = delete;
    BidirectionalGraph(BidirectionalGraph&& other) = default;
    BidirectionalGraph& operator=(const BidirectionalGraph& other) = delete;
    BidirectionalGraph& operator=(BidirectionalGraph&& other) = default;
    ~BidirectionalGraph() = default;

    /**
     * If the edge is forward, then the edge is added but additionally
     * a new edge reverse to the edge is with the same other properties 
     * is added to the graph as well. It is also set backward to imply that
     * the edge is reversed.
     */
    inline void AddEdge(Edge&& edge) {
        assert(!edge.IsBackward());
        if (edge.IsTwoway()) {
            g_.AddEdge(std::move(edge));
        } else {
            Edge backward_edge{edge};
            edge.SetForward();
            g_.AddEdge(std::move(edge));
            backward_edge.Reverse();
            backward_edge.SetBackward();
            g_.AddEdge(std::move(backward_edge));
        }
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

    unsigned_id_type GetMaxVertexId() {
        return g_.GetMaxVertexId();
    }
    
    unsigned_id_type GetMaxEdgeId() {
        return g_.GetMaxEdgeId();
    }

private:
    Graph g_;
};


}

#endif //ROUTING_BIDIRECTIONAL_GRAPH_H