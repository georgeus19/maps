#ifndef ROUTING_QUERY_ENDPOINTS_CREATOR_H
#define ROUTING_QUERY_ENDPOINTS_CREATOR_H

#include "routing/edges/basic_edge.h"

#include "utility/point.h"

namespace routing{
namespace query{

template <typename EndpointAlgorithmPolicy, typename EndpointEdgesCreator>
class EndpointsCreator {
public:
    EndpointsCreator(const EndpointAlgorithmPolicy& gp, const EndpointEdgesCreator& eec) 
        : graph_policy_(gp), endpoint_edges_creator_(eec) {}


    EndpointsCreator(EndpointAlgorithmPolicy&& gp, EndpointEdgesCreator&& eec) 
        : graph_policy_(std::move(gp)), endpoint_edges_creator_(std::move(eec)) {}

    void AddSourceEndpoint(const std::string& table_name, unsigned_id_type source_id, utility::Point source_location) {
        auto&& [edges, geometries] = endpoint_edges_creator_.CalculateEndpointEdges(table_name, source_id, source_location);
        source_edges_geometries_ = std::move(geometries);
        graph_policy_.AddSource(std::move(edges), source_id);
    }

    void AddTargetEndpoint(const std::string& table_name, unsigned_id_type target_id, utility::Point target_location) {
        auto&& [edges, geometries] = endpoint_edges_creator_.CalculateEndpointEdges(table_name, target_id, target_location);
        target_edges_geometries_ = std::move(geometries);
        graph_policy_.AddTarget(std::move(edges), target_id);
    }

    const std::string& GetSourceGeometry(unsigned_id_type edge_id) {
        return source_edges_geometries_.at(edge_id);
    }

    const std::string& GetTargetGeometry(unsigned_id_type edge_id) {
        return target_edges_geometries_.at(edge_id);
    }

private:
    EndpointAlgorithmPolicy graph_policy_;
    EndpointEdgesCreator endpoint_edges_creator_;

    std::vector<std::string> source_edges_geometries_;
    std::vector<std::string> target_edges_geometries_;
};






}
}

#endif // ROUTING_QUERY_ENDPOINTS_CREATOR_H