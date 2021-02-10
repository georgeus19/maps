#ifndef BACKEND_CONTRACTOR_H
#define BACKEND_CONTRACTOR_H
#include "routing/preprocessing/vertex_ordering.h"
#include "routing/edge.h"
#include <vector>
namespace routing {
namespace preprocessing {

template <typename Graph>
class VertexContractor {
    Graph & g_;
    unsigned_id_type free_edge_id_;
    VertexOrdering<Graph> ordering_;
    using Edge = Graph::E;
    using Vertex = Graph::V;
public:
    VertexContractor<Graph>(Graph & g, unsigned_id_type free_edge_id);

    void Contract();

    void ContractVertex(Vertex & vertex);

};

template <typename Graph>
VertexContractor<Graph>::VertexContractor(Graph &g, unsigned_id_type free_edge_id)
    : g_(g), free_edge_id_(free_edge_id), ordering_(VertexOrdering<Graph>(g)) {}

template <typename Graph>
void VertexContractor<Graph>::Contract() {

}

template <typename Graph>
void VertexContractor<Graph>::ContractVertex(Vertex & vertex) {
    auto&& outgoing = vertex.outgoing_edges_;
    auto&& ingoing = vertex.ingoing_edges_;
    std::vector<Edge> new_edges;
    
    double max_outgoing_length = std::max_element(outgoing.begin(), outgoing.end(), [](const Edge & a, const Edge & b) {
        return a.get_length() < b.get_length();
    })->get_length();
    // Create all new edges.
    for(auto&& ingoing_it = ingoing.cbegin(); ingoing_it != ingoing.cend(); ++ingoing_it) {

        Dijkstra<Graph> dijkstra{g_};
        double max_cost = max_outgoing_length + ingoing_it->get_length();
        auto&& end_condition = [=](Vertex * v) {
            return v->cost_ > max_cost;
        };
        dijkstra.Run(ingoing_it->from, end_condition);

        for(auto&& outgoing_it = outgoing.cbegin(); outgoing_it != outgoing.cend(); ++outgoing_it) {
            if (ingoing_it->get_length() + outgoing_it->get_length() <= g_.GetPathLength(outgoing_it->get_to)) {
                g_.AddEdge(typename Graph::Edge{++free_edge_id_, ingoing_it->from_, outgoing_it->to_, ingoing_it->length_ + outgoing_it->length_});
            }
        }
    }

}

template <typename Vertex, typename Edge>
class SearchSpaceLimit {
    const Vertex* start_;
    const Vertex* end_;
public:
    SearchSpaceLimit(const Vertex* start, const Vertex* end) : start_(start), end_(end) {}
    void operator()(Vertex & v) {

    }
};

}
}


#endif //BACKEND_CONTRACTOR_H
