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
    auto&& outgoing = vertex.get_edges();
    auto&& reverse_edges = vertex.get_reverse_edges();
    std::vector<Edge> new_edges;
    
    double max_outgoing_length = std::max_element(outgoing.begin(), outgoing.end(), [](const Edge & a, const Edge & b) {
        return a.get_length() < b.get_length();
    })->get_length();
    // Create all new edges.
    for(auto&& reverse_edges_it = reverse_edges.cbegin(); reverse_edges_it != reverse_edges.cend(); ++reverse_edges_it) {
        Dijkstra<Graph> dijkstra{g_};
        double max_cost = max_outgoing_length + reverse_edges_it->get_length();
        auto&& end_condition = [=](Vertex * v) {
            return v->get_cost() > max_cost;
        };
        dijkstra.Run(reverse_edges_it->get_to(), end_condition, [](Vertex* v) { return false; });

        for(auto&& outgoing_it = outgoing.cbegin(); outgoing_it != outgoing.cend(); ++outgoing_it) {
            bool edge_is_shortest_path = reverse_edges_it->get_length() + outgoing_it->get_length() <= dijkstra.GetPathLength(outgoing_it->get_to());
            if (edge_is_shortest_path) {
                g_.AddEdge(Edge{++free_edge_id_, reverse_edges_it->get_to(), outgoing_it->get_to(), reverse_edges_it->get_length() + outgoing_it->get_length()});
                g_.AddReverseEdge(Edge{++free_edge_id_, reverse_edges_it->get_to(), outgoing_it->get_to(), reverse_edges_it->get_length() + outgoing_it->get_length()});
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
