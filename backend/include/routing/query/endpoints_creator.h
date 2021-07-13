#ifndef ROUTING_QUERY_ENDPOINTS_CREATOR_H
#define ROUTING_QUERY_ENDPOINTS_CREATOR_H

#include "routing/edges/basic_edge.h"
#include "routing/exception.h"
#include "routing/types.h"

#include "routing/utility/point.h"

#include <utility>
#include <string>
#include <algorithm>


namespace routing{
namespace query{

template <typename EndpointAlgorithmPolicy, typename EndpointEdgesCreator>
class EndpointsCreator {
public:
    EndpointsCreator(const EndpointAlgorithmPolicy& gp, const EndpointEdgesCreator& eec) 
        : graph_policy_(gp), endpoint_edges_creator_(eec), free_endpoint_edge_id_(0), source_edges_geometries_(), target_edges_geometries_() {
        }

    EndpointsCreator(EndpointAlgorithmPolicy&& gp, EndpointEdgesCreator&& eec) 
        : graph_policy_(std::move(gp)), endpoint_edges_creator_(std::move(eec)), free_endpoint_edge_id_(0), source_edges_geometries_(), target_edges_geometries_()  {}

    void AddSourceEndpoint(const std::string& table_name, unsigned_id_type source_id, utility::Point source_location) {
        auto&& [edges, geometries] = endpoint_edges_creator_.CalculateEndpointEdges(table_name, source_id, source_location, free_endpoint_edge_id_);
        free_endpoint_edge_id_ += edges.size();
        source_edges_geometries_ = std::move(geometries);
        graph_policy_.AddSource(std::move(edges), source_id);
    }

    void AddTargetEndpoint(const std::string& table_name, unsigned_id_type target_id, utility::Point target_location) {
        auto&& [edges, geometries] = endpoint_edges_creator_.CalculateEndpointEdges(table_name, target_id, target_location, free_endpoint_edge_id_);
        free_endpoint_edge_id_ += edges.size();
        target_edges_geometries_ = std::move(geometries);
        graph_policy_.AddTarget(std::move(edges), target_id);
    }

    const std::string& GetSourceGeometry(unsigned_id_type edge_id) {
        return FindGeometry(source_edges_geometries_, edge_id);
    }

    const std::string& GetTargetGeometry(unsigned_id_type edge_id) {
        return FindGeometry(target_edges_geometries_, edge_id);
    }

private:
    EndpointAlgorithmPolicy graph_policy_;
    EndpointEdgesCreator endpoint_edges_creator_;

    unsigned_id_type free_endpoint_edge_id_;

    std::vector<std::pair<unsigned_id_type, std::string>> source_edges_geometries_;
    std::vector<std::pair<unsigned_id_type, std::string>> target_edges_geometries_;

    const std::string& FindGeometry(const std::vector<std::pair<unsigned_id_type, std::string>>& geometries, unsigned_id_type edge_id) {
        auto it = std::find_if(geometries.begin(), geometries.end(), [=](const std::pair<unsigned_id_type, std::string>& p){
            return edge_id == p.first;
        });
        if (it != geometries.end()) {
            return it->second;
        } else {
            throw InvalidArgumentException("No geometry of endpoint edge with id = " + std::to_string(edge_id) + " exists.");
        }
    }

};






}
}

#endif // ROUTING_QUERY_ENDPOINTS_CREATOR_H