#ifndef BACKEND_CONTRACTOR_H
#define BACKEND_CONTRACTOR_H
#include "routing/preprocessing/vertex_ordering.h"
#include "routing/edge.h"
#include <vector>
#include <set>
#include <queue>
#include <cassert>
namespace routing {
namespace preprocessing {

template <typename Graph>
class VertexContractor {
    Graph & g_;
    unsigned_id_type free_edge_id_;
    VertexOrdering<Graph> ordering_;
    using Edge = Graph::E;
    using Vertex = Graph::V;
    using PriorityQueue = std::priority_queue<std::pair<double, Vertex*>, std::vector<std::pair<double, Vertex*>>,  std::greater<double>>;
public:
    VertexContractor<Graph>(Graph & g, unsigned_id_type free_edge_id);

    void ContractGraph();

    void ContractVertex(Vertex & vertex);

    std::vector<Edge> FindShortcuts(Vertex & vertex);

private:

    void ContractMinVertex(PriorityQueue& q);

    void FindShortcuts(std::vector<Edge>& shortcuts, Vertex & contracted_vertex, const Edge & reversed_first_edge, double max_outgoing_length); 

    void AddShortcut(const Dijkstra<Graph>& dijkstra, unsigned_id_type start_vertex_id, unsigned_id_type end_vertex_id, double shortcut_length);

    void AddShortcuts(std::vector<Edge> shortcuts);

    size_t CalculateEdgeDifference(Vertex& vertex); 

    size_t CalculateEdgeDifference(Vertex& vertex, std::vector<Edge>& shortcuts);

    size_t CalculateDeletedNeighbours(Vertex& vertex);

    double CalculateContractionAttractivity(Vertex& vertex);

    double CalculateContractionAttractivity(Vertex& vertex, std::vector<Edge>& shortcuts);
};

template <typename Graph>
VertexContractor<Graph>::VertexContractor(Graph &g, unsigned_id_type free_edge_id)
    : g_(g), free_edge_id_(free_edge_id), ordering_(VertexOrdering<Graph>(g)) {}

template <typename Graph>
void VertexContractor<Graph>::ContractGraph() {
    PriorityQueue q;
    g_.ForEachVertex([&](Vertex& vertex) {
        double attractivity = CalculateContractionAttractivity(vertex);
        q.push(std::make_pair(attractivity, &vertex));
    });


    while(!q.empty()) {
        ContractMinVertex(q);
    }

}

template <typename Graph>
void VertexContractor<Graph>::ContractVertex(Vertex & vertex) {
    std::vector<Edge> shortcuts = FindShortcuts(vertex);
    AddShortcuts(shortcuts);
    vertex.SetContracted();
}

template <typename Graph>
std::vector<typename VertexContractor<Graph>::Edge> VertexContractor<Graph>::FindShortcuts(Vertex & vertex) {
    std::vector<Edge> shortcuts{};
    double max_outgoing_length = std::max_element(vertex.get_edges().begin(), vertex.get_edges().end(), [](const Edge & a, const Edge & b) {
        return a.get_length() < b.get_length();
    })->get_length();

    for(auto&& reverse_edge : vertex.get_reverse_edges()) {
        FindShortcuts(shortcuts, vertex, reverse_edge, max_outgoing_length);
    }
    return shortcuts;
}

template <typename Graph>
void VertexContractor<Graph>::ContractMinVertex(VertexContractor<Graph>::PriorityQueue& q) {
    assert(!q.empty());

    std::vector<Edge> shortcuts;
    Vertex* v;
    if (q.size() != 1) {
        double priority_threshold;
        double new_priority;
        while(true) {
            auto&& pair = q.top();
            q.pop();
            v = pair->second;
            double priority_threshold = q.top().first;
            shortcuts = FindShortcuts(*v);
            double new_priority = CalculateContractionAttractivity(*v, shortcuts);
            if (new_priority <= priority_threshold) {
                break;
            }
            q.push(std::make_pair(new_priority, v));
        }
    } else {
        auto&& pair = q.top();
        q.pop();
        v = pair->second;
        shortcuts = FindShortcuts(*v);
    }

    AddShortcuts(shortcuts);
    v.SetContracted();
}

template <typename Graph>
void VertexContractor<Graph>::FindShortcuts(std::vector<Edge>& shortcuts, Vertex & contracted_vertex, const Edge & reversed_first_edge, double max_outgoing_length) {
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
        double shortcut_length = reversed_first_edge.get_length() + second_edge.get_length();
        unsigned_id_type end_vertex_id = second_edge.get_to();
        double path_length = dijkstra.GetPathLength(end_vertex_id);
        
        if (g_.GetVertex(end_vertex_id)->IsContracted() || shortcut_length > path_length) {
            continue;
        }
        shortcuts.push_back(Edge{++free_edge_id_, start_vertex_id, end_vertex_id, shortcut_length});
    }
}

template <typename Graph>
void VertexContractor<Graph>::AddShortcuts(std::vector<Edge> shortcuts) {
    for(auto&& edge : shortcuts) {
        Edge reversed_edge{edge};
        g_.AddEdge(std::move(edge));
        g_.AddReverseEdge(std::move(reversed_edge));
    }
}

template <typename Graph>
size_t VertexContractor<Graph>::CalculateEdgeDifference(Vertex& vertex) {
    return CalculateEdgeDifference(vertex, FindShortcuts(vertex));
}

template <typename Graph>
size_t VertexContractor<Graph>::CalculateEdgeDifference(Vertex& vertex, std::vector<Edge>& shortcuts) {
    return shortcuts.size() - vertex.edges().size();
}

template <typename Graph>
size_t VertexContractor<Graph>::CalculateDeletedNeighbours(Vertex& vertex) {
    size_t deleted_neighbours = 0;
    for(auto&& edge : vertex.edges()) {
        Vertex* neighbour = g_.GetVertex(edge.get_to());
        if (!neighbour->IsContracted()) {
            ++deleted_neighbours;
        }   
    }
    return deleted_neighbours;
}

template <typename Graph>
double VertexContractor<Graph>::CalculateContractionAttractivity(Vertex& vertex) {
    return (double)CalculateEdgeDifference(vertex) + CalculateDeletedNeighbours(vertex);
}

template <typename Graph>
double VertexContractor<Graph>::CalculateContractionAttractivity(Vertex& vertex, std::vector<Edge>& shortcuts) {
    return (double)CalculateEdgeDifference(vertex, shortcuts) + CalculateDeletedNeighbours(vertex);
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
