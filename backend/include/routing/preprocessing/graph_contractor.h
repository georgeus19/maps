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
    Graph & g_;
    VertexMeasures<Graph> vertex_measures_;
    unsigned_id_type free_ordering_rank_;

    void AddShortcuts(std::vector<Edge> shortcuts);

   
};

template <typename Graph>
GraphContractor<Graph>::GraphContractor(Graph &g, const ContractionParameters& parameters)
    : g_(g), vertex_measures_(VertexMeasures<Graph>{g, parameters}), free_ordering_rank_(0) {}

template <typename Graph>
void GraphContractor<Graph>::ContractGraph() {
    free_ordering_rank_ = 0;
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
    vertex.set_ordering_rank(++free_ordering_rank_);
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
    v->set_ordering_rank(++free_ordering_rank_);
}

template <typename Graph>
GraphContractor<Graph>::PriorityQueue GraphContractor<Graph>::CalculateContractionPriority() {
    PriorityQueue q;
    g_.ForEachVertex([&](Vertex& vertex) {
        double attractivity = vertex_measures_.CalculateContractionAttractivity(vertex);
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

}
}


#endif //BACKEND_GRAPH_CONTRACTOR_H
