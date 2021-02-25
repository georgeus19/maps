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

private:
    void AddShortcuts(Vertex & contracted_vertex, const Edge & reversed_first_edge, double max_outgoing_length); 

    void AddShortcut(const Dijkstra<Graph>& dijkstra, unsigned_id_type start_vertex_id, unsigned_id_type end_vertex_id, double shortcut_length);

};

template <typename Graph>
VertexContractor<Graph>::VertexContractor(Graph &g, unsigned_id_type free_edge_id)
    : g_(g), free_edge_id_(free_edge_id), ordering_(VertexOrdering<Graph>(g)) {}

template <typename Graph>
void VertexContractor<Graph>::Contract() {

}

template <typename Graph>
void VertexContractor<Graph>::ContractVertex(Vertex & vertex) {
    double max_outgoing_length = std::max_element(vertex.get_edges().begin(), vertex.get_edges().end(), [](const Edge & a, const Edge & b) {
        return a.get_length() < b.get_length();
    })->get_length();

    for(auto&& reverse_edge : vertex.get_reverse_edges()) {
        AddShortcuts(vertex, reverse_edge, max_outgoing_length);
    }
    vertex.SetContracted();
}

template <typename Graph>
void VertexContractor<Graph>::AddShortcuts(Vertex & contracted_vertex, const Edge & reversed_first_edge, double max_outgoing_length) {
    unsigned_id_type start_vertex_id = reversed_first_edge.get_to();
    Vertex* start_vertex = g_.GetVertex(start_vertex_id);

    if (start_vertex->IsContracted()) {
        return;
    }
    Dijkstra<Graph> dijkstra{g_};
    double max_cost = max_outgoing_length + reversed_first_edge.get_length();
    auto&& end_condition = [=](Vertex * v) {
        return v->get_cost() > max_cost;
    };
    dijkstra.Run(start_vertex_id, end_condition, [](Vertex* v) { return v->IsContracted(); });

    for(auto&& second_edge : contracted_vertex.get_edges()) {
        AddShortcut(dijkstra, start_vertex_id, second_edge.get_to(), reversed_first_edge.get_length() + second_edge.get_length());
    }
}

template <typename Graph>
void VertexContractor<Graph>::AddShortcut(const Dijkstra<Graph>& dijkstra, unsigned_id_type start_vertex_id, unsigned_id_type end_vertex_id, double shortcut_length) {
    if (g_.GetVertex(end_vertex_id)->IsContracted()) {
        return;
    }
    bool edge_is_shortest_path = shortcut_length <= dijkstra.GetPathLength(end_vertex_id);
    if (edge_is_shortest_path) {
        g_.AddEdge(Edge{++free_edge_id_, start_vertex_id, end_vertex_id, shortcut_length});
        g_.AddReverseEdge(Edge{++free_edge_id_, start_vertex_id, end_vertex_id, shortcut_length});
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
