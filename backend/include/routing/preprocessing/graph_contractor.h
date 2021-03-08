#ifndef BACKEND_GRAPH_CONTRACTOR_H
#define BACKEND_GRAPH_CONTRACTOR_H
#include "routing/preprocessing/vertex_measures.h"
#include "routing/edges/basic_edge.h"
#include "routing/preprocessing/contraction_parameters.h"
#include <vector>
#include <set>
#include <queue>
#include <cassert>
namespace routing {
namespace preprocessing {

template <typename Graph>
class GraphContractor {
    Graph & g_;
    VertexMeasures<Graph> vertex_measures_;
    using Edge = Graph::E;
    using Vertex = Graph::V;
public:
    using PriorityQueue = std::priority_queue<std::pair<double, Vertex*>, std::vector<std::pair<double, Vertex*>>,  std::greater<std::pair<double, Vertex*>>>;
    GraphContractor<Graph>(Graph & g, const ContractionParameters& parameters);

    void ContractGraph();

    void ContractVertex(Vertex & vertex);

    void ContractMinVertex(PriorityQueue& q);

    PriorityQueue CalculateContractionPriority();
private:

    void AddShortcuts(std::vector<Edge> shortcuts);

   
};

template <typename Graph>
GraphContractor<Graph>::GraphContractor(Graph &g, const ContractionParameters& parameters)
    : g_(g), vertex_measures_(VertexMeasures<Graph>{g, parameters}) {}

template <typename Graph>
void GraphContractor<Graph>::ContractGraph() {
    PriorityQueue q = CalculateContractionPriority();

    while(!q.empty()) {
        ContractMinVertex(q);
    }

}

template <typename Graph>
void GraphContractor<Graph>::ContractVertex(Vertex & vertex) {
    std::vector<Edge> shortcuts = vertex_measures_.FindShortcuts(vertex);
    AddShortcuts(shortcuts);
    vertex.SetContracted();
}



template <typename Graph>
void GraphContractor<Graph>::ContractMinVertex(GraphContractor<Graph>::PriorityQueue& q) {
    assert(!q.empty());

    std::vector<Edge> shortcuts;
    Vertex* v;
    if (q.size() != 1) {
        double priority_threshold;
        double new_priority;
        while(true) {
            v = q.top().second;
            q.pop();
            double priority_threshold = q.top().first;
            shortcuts = vertex_measures_.FindShortcuts(*v);
            double new_priority = vertex_measures_.CalculateContractionAttractivity(*v, shortcuts);
            if (new_priority <= priority_threshold) {
                break;
            }
            q.push(std::make_pair(new_priority, v));
        }
    } else {
        v = q.top().second;
        shortcuts = vertex_measures_.FindShortcuts(*v);
        q.pop();
    }

    AddShortcuts(shortcuts);
    v->SetContracted();
}

template <typename Graph>
GraphContractor<Graph>::PriorityQueue GraphContractor<Graph>::CalculateContractionPriority() {
    PriorityQueue q;
    g_.forEachVertex([&](Vertex& vertex) {
        double attractivity = CalculateContractionAttractivity(vertex);
        q.push(std::make_pair(attractivity, &vertex));
    });
    return q;
}


template <typename Graph>
void GraphContractor<Graph>::AddShortcuts(std::vector<Edge> shortcuts) {
    for(auto&& edge : shortcuts) {
        Edge reversed_edge{edge};
        g_.AddEdge(std::move(edge));
        g_.AddReverseEdge(std::move(reversed_edge));
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


#endif //BACKEND_GRAPH_CONTRACTOR_H
