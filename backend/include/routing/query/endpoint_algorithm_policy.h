#ifndef ROUTING_QUERY_ENDPOINT_ALGORITHM_POLICY_H
#define ROUTING_QUERY_ENDPOINT_ALGORITHM_POLICY_H

#include "routing/types.h"

#include <functional>

namespace routing{
namespace query{

/**
 * EndpointAlgorithmPolicyContractionHierarchies is a policy class that defines how endpoint vertices
 * and their edges should be added to a road graph.
 * 
 * Since CH uses Bidirectional Dijkstra as query algorithm,
 * the target endpoint vertex must have backward edges so that
 * backward search in Bidirectional Dijkstra can reach its neighbours.
 */
template <typename Graph, typename EdgeRangePolicy>
class EndpointAlgorithmPolicyContractionHierarchies{
public:

    EndpointAlgorithmPolicyContractionHierarchies(Graph& graph, const EdgeRangePolicy& ers)
        : graph_(std::ref(graph)), edge_range_policy_(ers) {}

    EndpointAlgorithmPolicyContractionHierarchies(Graph& graph, EdgeRangePolicy&& ers)
        : graph_(std::ref(graph)), edge_range_policy_(std::move(ers)) {}

    void AddSource(std::vector<typename Graph::Edge>&& edges, unsigned_id_type source_id) {
        typename Graph::Vertex source_vertex{source_id, edge_range_policy_.CreateEdgeRange(std::move(edges)), 0}; 
        graph_.get().AddVertex(std::move(source_vertex));
    }

    void AddTarget(std::vector<typename Graph::Edge>&& edges, unsigned_id_type target_id) {
        for(auto&& edge : edges) {
            edge.SetBackward();
        }
        typename Graph::Vertex target_vertex{target_id, edge_range_policy_.CreateEdgeRange(std::move(edges)), 1}; 
        graph_.get().AddVertex(std::move(target_vertex));
    }

private:
    std::reference_wrapper<Graph> graph_;
    EdgeRangePolicy edge_range_policy_;
};

/**
 * EndpointAlgorithmPolicyDijkstra is a policy class that defines how endpoint vertices
 * and their edges should be added to a road graph.
 * 
 * Adding target endpoint vertex requires that its edges are added to its neighbours
 * so that it can be reach from source.
 */
template <typename Graph, typename EdgeRangePolicy>
class EndpointAlgorithmPolicyDijkstra{
public:

    EndpointAlgorithmPolicyDijkstra(Graph& graph, const EdgeRangePolicy& ers)
        : graph_(std::ref(graph)), edge_range_policy_(ers) {}

    EndpointAlgorithmPolicyDijkstra(Graph& graph, EdgeRangePolicy&& ers)
        : graph_(std::ref(graph)), edge_range_policy_(std::move(ers)) {}

    void AddSource(std::vector<typename Graph::Edge>&& edges, unsigned_id_type source_id) {
        typename Graph::Vertex source_vertex{source_id, edge_range_policy_.CreateEdgeRange(std::move(edges))}; 
        graph_.get().AddVertex(std::move(source_vertex));
    }

    void AddTarget(std::vector<typename Graph::Edge>&& edges, unsigned_id_type target_id) {
         for(auto&& edge : edges) {
            edge.Reverse();
            auto&& immutable_neighbour = graph_.get().GetVertex(edge.get_from());
            std::vector<typename Graph::Edge> neighbour_edges{};
            neighbour_edges.insert(neighbour_edges.end(), immutable_neighbour.get_edges().begin(), immutable_neighbour.get_edges().end());
            neighbour_edges.push_back(std::move(edge));
            typename Graph::Vertex neighbour{immutable_neighbour.get_uid(), edge_range_policy_.CreateEdgeRange(std::move(neighbour_edges))};
            graph_.get().AddVertex(std::move(neighbour));
        }
        typename Graph::Vertex target_vertex{target_id, edge_range_policy_.CreateEdgeRange(std::vector<typename Graph::Edge>{})}; 
        graph_.get().AddVertex(std::move(target_vertex));
    }

private:
    std::reference_wrapper<Graph> graph_;
    EdgeRangePolicy edge_range_policy_;
};



}
}

#endif // ROUTING_QUERY_ENDPOINT_ALGORITHM_POLICY_H