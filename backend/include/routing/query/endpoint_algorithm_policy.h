#ifndef ROUTING_QUERY_GRAPH_POLICY_H
#define ROUTING_QUERY_GRAPH_POLICY_H

#include <functional>

namespace routing{
namespace query{

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



}
}

#endif // ROUTING_QUERY_GRAPH_POLICY_H