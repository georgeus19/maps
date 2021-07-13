#ifndef ROUTING_PREPROCESSING_GRAPH_CONTRACTOR_H
#define ROUTING_PREPROCESSING_GRAPH_CONTRACTOR_H

#include "routing/preprocessing/vertex_measures.h"
#include "routing/edges/basic_edge.h"
#include "routing/preprocessing/contraction_parameters.h"
#include "routing/preprocessing/shortcut_finder.h"
#include "routing/types.h"

#include <vector>
#include <set>
#include <queue>
#include <cassert>
namespace routing {
namespace preprocessing {

template <typename Graph>
class GraphContractor {
    
    using Edge = typename Graph::Edge;
    using Vertex = typename Graph::Vertex;
public:
    using PriorityQueue = std::priority_queue<
        std::pair<float, unsigned_id_type>,
        std::vector<std::pair<float, unsigned_id_type>>,
        std::greater<std::pair<float, unsigned_id_type>>
    >;
    GraphContractor<Graph>(Graph& g, const ContractionParameters& parameters, unsigned_id_type free_edge_id);

    void ContractGraph();

    void ContractVertex(Vertex & vertex);

    void ContractMinVertex(PriorityQueue& q);

    PriorityQueue CalculateContractionPriority();
private:
    Graph & g_;
    ShortcutFinder<Graph> shortcut_finder_;
    VertexMeasures<Graph> vertex_measures_;
    unsigned_id_type free_edge_id_;
    unsigned_id_type free_ordering_rank_;

    void AddShortcuts(std::vector<Edge>&& shortcuts);

    float CalculateOverlayGraphAverageDegree() const;
};

template <typename Graph>
GraphContractor<Graph>::GraphContractor(Graph &g, const ContractionParameters& parameters, unsigned_id_type free_edge_id)
    : g_(g), shortcut_finder_(g, parameters), vertex_measures_(g, parameters), free_edge_id_(free_edge_id), free_ordering_rank_(1) {}

template <typename Graph>
void GraphContractor<Graph>::ContractGraph() {
    free_ordering_rank_ = 1;
    PriorityQueue q = CalculateContractionPriority();
    std::cout << "Contraction starting - queue completed." << std::endl;
    size_t count = 0;
    g_.ForEachVertex([&](Vertex& vertex) {
           ++count;
    });
    while(!q.empty()) {
        if (count % 10000 == 0) {
            std::cout << count << " vertices left, average degree = " << std::endl; //CalculateOverlayGraphAverageDegree() << std::endl;
        } 
        // if (count < 50000 && count % 1000 == 0) {
        //     std::cout << count << " vertices left, average degree = " << CalculateOverlayGraphAverageDegree() << std::endl;
        // } 
        // if (count < 1000 && count % 10 == 0) {
        //     std::cout << count << " vertices left, average degree = " << CalculateOverlayGraphAverageDegree() << std::endl;
        // }
        // if (count < 10) {
        //     std::cout << count << " vertices left, average degree = " << CalculateOverlayGraphAverageDegree() << std::endl;
        // }
        ContractMinVertex(q);
        --count;
    }

}

template <typename Graph>
void GraphContractor<Graph>::ContractVertex(Vertex & vertex) {
    std::vector<Edge> shortcuts = shortcut_finder_.FindShortcuts(vertex);
    AddShortcuts(std::move(shortcuts));
    // vertex.SetContracted();
    vertex.set_ordering_rank(++free_ordering_rank_);
}



template <typename Graph>
void GraphContractor<Graph>::ContractMinVertex(GraphContractor<Graph>::PriorityQueue& q) {
    assert(!q.empty());

    std::vector<Edge> shortcuts;
    unsigned_id_type vertex_id;
    if (q.size() != 1) {
        while(true) {
            vertex_id = q.top().second;
            auto&& vertex = g_.GetVertex(vertex_id);
            q.pop();
            float priority_threshold = q.top().first;
            shortcuts = shortcut_finder_.FindShortcuts(vertex);
            float new_priority = vertex_measures_.CalculateContractionAttractivity(vertex, shortcuts);
            if (new_priority <= priority_threshold) {
                break;
            }
            q.push(std::make_pair(new_priority, vertex_id));
        }
    } else {
        vertex_id = q.top().second;
        auto&& vertex = g_.GetVertex(vertex_id);
        shortcuts = shortcut_finder_.FindShortcuts(vertex);
        q.pop();
    }

    AddShortcuts(std::move(shortcuts));
    auto&& contracted_vertex = g_.GetVertex(vertex_id);
    // contracted_vertex.SetContracted();
    contracted_vertex.set_ordering_rank(++free_ordering_rank_);
}

template <typename Graph>
typename GraphContractor<Graph>::PriorityQueue GraphContractor<Graph>::CalculateContractionPriority() {
    PriorityQueue q;
    g_.ForEachVertex([&](Vertex& vertex) {
        if (!vertex.IsContracted()) {
            float attractivity = vertex_measures_.CalculateContractionAttractivity(vertex);
            q.push(std::make_pair(attractivity, vertex.get_uid()));
        }
        
    });
    return q;
}

template <typename Graph>
void GraphContractor<Graph>::AddShortcuts(std::vector<Edge>&& shortcuts) {
    for(auto&& edge : shortcuts) {
        edge.set_uid(++free_edge_id_);
        g_.AddEdge(std::move(edge));
    }
}


template <typename Graph>
float GraphContractor<Graph>::CalculateOverlayGraphAverageDegree() const {
    size_t deg = 0;
    size_t count = 0;
    g_.ForEachVertex([&](Vertex& vertex) {
        if (!vertex.IsContracted()) {
            vertex.ForEachEdge([&](Edge& e) {
                if (!g_.GetVertex(e.get_to()).IsContracted()) {
                   ++deg;
                }
            });
           ++count;
        }
        
    });
    return ((float)deg) / ((float)count);
}



}
}
#endif //ROUTING_PREPROCESSING_GRAPH_CONTRACTOR_H