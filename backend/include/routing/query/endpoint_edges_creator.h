#ifndef ROUTING_QUERY_ENPOINT_EDGES_CREATOR_H
#define ROUTING_QUERY_ENPOINT_EDGES_CREATOR_H

#include "routing/edges/basic_edge.h"
#include "routing/exception.h"
#include "routing/types.h"

#include "routing/database/database_helper.h"
#include "routing/database/db_graph.h"

#include "routing/utility/comparison.h"

#include <unordered_map>
#include <string>
#include <functional>
#include <utility>

namespace routing{
namespace query{

/**
 * EndpointEdgesCreator handles creating edges from endpoint (defined by lat loncoordinates)
 * to closest intersection (ie vertex). 
 * 
 * It finds the geographically closest edge to the endpoint and splits it into two to 
 * create edges to endpoint vertex. It also creates geometries for these edges.
 */
template <typename EdgeFactory, typename Graph>
class EndpointEdgesCreator {
public:
    EndpointEdgesCreator();
    EndpointEdgesCreator(Graph& graph, database::DatabaseHelper& d, database::DbGraph* db_graph, const EdgeFactory& edge_factory);

    EndpointEdgesCreator(const EndpointEdgesCreator& other) = default;
    EndpointEdgesCreator(EndpointEdgesCreator&& other) = default;
    EndpointEdgesCreator& operator=(const EndpointEdgesCreator& other) = default;
    EndpointEdgesCreator& operator=(EndpointEdgesCreator&& other) = default;
    ~EndpointEdgesCreator() = default;
    /**
     * Create new edges that are from the closest points of the endpoint's closest graph edge to the endpoint to
     * endpoint's closest graph edge endpoints(real intersection). Basically split the closest edge to endpoint
     * to multiple segments. Geometries of new segments are saved and new edges are created from segments
     * and returned in vector.
     *
     * @return vector of new edges and their geometries.
     */
    std::pair<std::vector<typename EdgeFactory::Edge>, std::vector<std::pair<unsigned_id_type, std::string>>> CalculateEndpointEdges(
        const std::string& table_name, unsigned_id_type endpoint_id, utility::Point p, unsigned_id_type free_edge_id);

private:

    std::reference_wrapper<Graph> graph_;

    std::reference_wrapper<database::DatabaseHelper> d_;

    database::DbGraph* db_graph_;

    EdgeFactory edge_factory_;

    /**
     * Indices to the output of the SQL query used to retrieve the closest edge, etc..
     */
    static const size_t kAdjEdgeFrom = 0;
    static const size_t kAdjEdgeTo = 1;
    static const size_t kClosestEdgeFrom = 2;
    static const size_t kClosestEdgeTo = 3;
    static const size_t kClosestEdgeUid = 4;
    static const size_t kSegmentRelativeLength = 5;
    static const size_t kSegmentGeometry = 6;
    static const size_t kSegmentIntersectsAdjacent = 7;

    class EdgeInputData{
    public:

        EdgeInputData(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, float length)
            : uid_(uid), from_(from), to_(to), length_(length) {}

        unsigned_id_type GetUid() const {
            return uid_;
        }

        unsigned_id_type GetFrom() const {
            return from_;
        }

        unsigned_id_type GetTo() const {
            return to_;
        }

        float GetLength() const {
            return length_;
        }

        bool GetUndirected() const {
            return false;
        }

    private:
        unsigned_id_type uid_;
        unsigned_id_type from_;
        unsigned_id_type to_;
        float length_;
    };

    /**
     * Create new edges from segment and add them to `result_edges` vector.
     *
     * @param r Db row representing segment.
     * @param result_edges Output value where new edges are added to.
     * @param edge_id Edge id which is free to use as well as the +1 id.
     * @param endpoint_id Node id which is free to use and which is one endpoint of the segment.
     * @param intersection_id Id of original intersection that will serve as the other endpoint.
     */
    void SaveEdge(database::DbRow& r, std::vector<typename EdgeFactory::Edge>& result_edges, std::vector<std::pair<unsigned_id_type, std::string>>& result_geometries,
        typename Graph::Edge& closest_edge, unsigned_id_type endpoint_id, unsigned_id_type intersection_id, unsigned_id_type free_edge_id);

    typename Graph::Edge& GetEdge(unsigned_id_type edge_id, unsigned_id_type edge_from, unsigned_id_type edge_to);
};

template <typename EdgeFactory, typename Graph>
EndpointEdgesCreator<EdgeFactory, Graph>::EndpointEdgesCreator(Graph& graph, database::DatabaseHelper& d, database::DbGraph* db_graph, const EdgeFactory& edge_factory)
            : graph_(std::ref(graph)), d_(std::ref(d)), db_graph_(db_graph), edge_factory_(edge_factory) {}

template <typename EdgeFactory, typename Graph>
std::pair<std::vector<typename EdgeFactory::Edge>, std::vector<std::pair<unsigned_id_type, std::string>>> EndpointEdgesCreator<EdgeFactory, Graph>::CalculateEndpointEdges(
        const std::string& table_name, unsigned_id_type endpoint_id, utility::Point p, unsigned_id_type free_edge_id) {
    std::vector<database::DbRow> rows = d_.get().GetClosestSegments(p, table_name, db_graph_);

    // Endpoint has a closest edge that has no neighbours.
    if (rows.size() == 0) {
        throw RouteNotFoundException{"Route cannot be found - endpoint edge has no neighbours."};
    }


    std::vector<typename EdgeFactory::Edge> result_edges{};
    std::vector<std::pair<unsigned_id_type, std::string>> result_geometries{};

    unsigned_id_type segment_intersecting_adjacent_index = 0;
    if (rows.size() > 1) {
        segment_intersecting_adjacent_index = ((rows[0].get<bool>(kSegmentIntersectsAdjacent)) ? 0 : 1);
    }

    // Create edges from the segment that intersects adjacent edge from sql query.
    auto&& segment_row = rows[segment_intersecting_adjacent_index];
    unsigned_id_type adj_edge_from = segment_row.get<unsigned_id_type>(kAdjEdgeFrom);
    unsigned_id_type adj_edge_to = segment_row.get<unsigned_id_type>(kAdjEdgeTo);
    unsigned_id_type closest_edge_from = segment_row.get<unsigned_id_type>(kClosestEdgeFrom);
    unsigned_id_type closest_edge_to = segment_row.get<unsigned_id_type>(kClosestEdgeTo);
    unsigned_id_type closest_edge_uid = segment_row.get<unsigned_id_type>(kClosestEdgeUid);

    // Determine the segment's endpoint uid (it is one of closest edge intersections)
    // The other endpoint is newly created point (split point).
    bool closest_edge_from_used = false;
    unsigned_id_type adjacent_intersection = 0;
    if (closest_edge_from == adj_edge_from || closest_edge_from ==  adj_edge_to) {
        closest_edge_from_used = true;
        adjacent_intersection = closest_edge_from;
    } else {
        adjacent_intersection = closest_edge_to; 
    }
    
    auto&& closest_edge = GetEdge(closest_edge_uid, closest_edge_from, closest_edge_to);

    SaveEdge(segment_row, result_edges, result_geometries, closest_edge, endpoint_id, adjacent_intersection, free_edge_id);
    ++free_edge_id;

    if (rows.size() == 1) {
        return std::make_pair(result_edges, result_geometries);
    }

    // Create edges for the other segment.
    unsigned_id_type other_segment_index = 1 - segment_intersecting_adjacent_index;

    unsigned_id_type other_intersection = ((closest_edge_from_used) ? closest_edge_to : closest_edge_from);
    SaveEdge(rows[other_segment_index], result_edges, result_geometries, closest_edge, endpoint_id, other_intersection, free_edge_id);
    ++free_edge_id;
    return std::make_pair(result_edges, result_geometries);
}

template <typename EdgeFactory, typename Graph>
void EndpointEdgesCreator<EdgeFactory, Graph>::SaveEdge(database::DbRow& r, std::vector<typename EdgeFactory::Edge>& result_edges, std::vector<std::pair<unsigned_id_type, std::string>>& result_geometries,
    typename Graph::Edge& closest_edge, unsigned_id_type endpoint_id, unsigned_id_type intersection_id, unsigned_id_type free_edge_id) {
    float length = closest_edge.get_length() * r.get<float>(kSegmentRelativeLength);
    std::string geometry = r.get<std::string>(kSegmentGeometry);
    result_edges.push_back(edge_factory_.Create(EdgeInputData{free_edge_id, endpoint_id, intersection_id, length}));
    result_geometries.push_back(std::make_pair(free_edge_id, geometry));
}

template <typename EdgeFactory, typename Graph>
typename Graph::Edge& EndpointEdgesCreator<EdgeFactory, Graph>::GetEdge(unsigned_id_type edge_id, unsigned_id_type edge_from, unsigned_id_type edge_to) {
    auto&& from_vertex = graph_.get().GetVertex(edge_from);
    for(auto&& edge : from_vertex.get_edges()) {
        if (edge_id == edge.get_uid()) {
            return edge;
        }
    }

    auto&& to_vertex = graph_.get().GetVertex(edge_to);
    for(auto&& edge : to_vertex.get_edges()) {
        if (edge_id == edge.get_uid()) {
            return edge;
        }
    }

    throw EdgeNotFoundException{"Closest edge not in graph"};
}






}
}
#endif //ROUTING_QUERY_ENPOINT_EDGES_CREATOR_H