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
    using PriorityQueue = std::priority_queue<std::pair<double, unsigned_id_type>, std::vector<std::pair<double, unsigned_id_type>>,  std::greater<std::pair<double, unsigned_id_type>>>;
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

    double CalculateOverlayGraphAverageDegree() const;

   
};

template <typename Graph>
GraphContractor<Graph>::GraphContractor(Graph &g, const ContractionParameters& parameters)
    : g_(g), vertex_measures_(VertexMeasures<Graph>{g, parameters}), free_ordering_rank_(0) {}

template <typename Graph>
void GraphContractor<Graph>::ContractGraph() {
    free_ordering_rank_ = 0;
    PriorityQueue q = CalculateContractionPriority();
    std::cout << "Contraction starting - queue completed." << std::endl;
    size_t count = 0;
    g_.ForEachVertex([&](Vertex& vertex) {
           ++count;
    });
    while(!q.empty()) {
        if (count % 10000 == 0) {
            std::cout << count << " vertices left, average degree = " << CalculateOverlayGraphAverageDegree() << std::endl;
        } 
        if (count < 50000 && count % 1000 == 0) {
            std::cout << count << " vertices left, average degree = " << CalculateOverlayGraphAverageDegree() << std::endl;
        } 
        if (count < 1000 && count % 10 == 0) {
            std::cout << count << " vertices left, average degree = " << CalculateOverlayGraphAverageDegree() << std::endl;
        }
        if (count < 10) {
            std::cout << count << " vertices left, average degree = " << CalculateOverlayGraphAverageDegree() << std::endl;
        }
        ContractMinVertex(q);
        --count;
    }
    std::cout << count << " vertices left, average degree = " << CalculateOverlayGraphAverageDegree() << std::endl;

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
    size_t repeat = 0;

    std::vector<Edge> shortcuts;
    unsigned_id_type vertex_id;
    if (q.size() != 1) {
        double priority_threshold;
        double new_priority;
        while(true) {
            vertex_id = q.top().second;
            auto&& vertex = g_.GetVertex(vertex_id);
            q.pop();
            double priority_threshold = q.top().first;
            shortcuts = vertex_measures_.FindShortcuts(vertex);
            double new_priority = vertex_measures_.CalculateContractionAttractivity(vertex, shortcuts);
            if (new_priority <= priority_threshold) {
                break;
            }
            q.push(std::make_pair(new_priority, vertex_id));
            ++repeat;
        }
    } else {
        vertex_id = q.top().second;
        auto&& vertex = g_.GetVertex(vertex_id);
        shortcuts = vertex_measures_.FindShortcuts(vertex);
        q.pop();
    }
    // if (repeat > 10) {
    //     std::cout << " lazy update repeat is " << repeat << std::endl;
    // }

    AddShortcuts(shortcuts);
    auto&& contracted_vertex = g_.GetVertex(vertex_id);
    contracted_vertex.SetContracted();
    contracted_vertex.set_ordering_rank(++free_ordering_rank_);
}

template <typename Graph>
GraphContractor<Graph>::PriorityQueue GraphContractor<Graph>::CalculateContractionPriority() {
    PriorityQueue q;
    size_t count = 0;
    g_.ForEachVertex([&](Vertex& vertex) {
        if (!vertex.IsContracted()) {
            ++count;
            if (count % 10000 == 0) {

                std::cout << count << " CalculateContractionPriority iterations" << std::endl;
            }
            double attractivity = vertex_measures_.CalculateContractionAttractivity(vertex);
            q.push(std::make_pair(attractivity, vertex.get_osm_id()));
        }
        
    });
    return q;
}


template <typename Graph>
void GraphContractor<Graph>::AddShortcuts(std::vector<Edge> shortcuts) {
    for(auto&& edge : shortcuts) {
        Edge reversed_edge{edge};
        g_.AddEdge(std::move(edge));
        // g_.AddReverseEdge(std::move(reversed_edge));
    }
}


template <typename Graph>
double GraphContractor<Graph>::CalculateOverlayGraphAverageDegree() const {
    size_t deg = 0;
    size_t count = 0;
    g_.ForEachVertex([&](Vertex& vertex) {
        if (!vertex.IsContracted()) {
           deg += vertex.get_edges().size();
           ++count;
        }
        
    });
    return ((double)deg) / ((double)count);
}

}
}


#endif //BACKEND_GRAPH_CONTRACTOR_H
