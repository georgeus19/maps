#ifndef ROUTING_QUERY_ENPOINT_EDGES_CREATOR_H
#define ROUTING_QUERY_ENPOINT_EDGES_CREATOR_H

#include "routing/edges/basic_edge.h"
#include "routing/exception.h"

#include "routing/database/database_helper.h"
#include "routing/database/db_graph.h"

#include <unordered_map>
#include <string>
#include <functional>
#include <utility>

namespace routing{
namespace query{

/**
 * EndpointEdgesCreator handles creating edges from endpoint to closest intersection - Splitting
 * the closest edge to multiple segments so that routing is done more accurately.
 *
 * It also stores the segments' geometries which can be later queried for to construct
 * the geometry of the whole route.
 *
 * This is the implementation of EndpointHandler for graph with BasicEdge edges.
 */
template <typename EdgeFactory>
class EndpointEdgesCreator {
public:
    EndpointEdgesCreator();
    EndpointEdgesCreator(database::DatabaseHelper& d, database::DbGraph* db_graph, const EdgeFactory& edge_factory);

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
     * @param p Endpoint of the route - start/ end.
     * @param table_name Table where edges are stored.
     * @param d DatabaseHandler instance which is used to fetch data.
     * @return Vector of edges created from new segments.
     */
    std::pair<std::vector<typename EdgeFactory::Edge>, std::vector<std::pair<unsigned_id_type, std::string>>> CalculateEndpointEdges(
        const std::string& table_name, unsigned_id_type endpoint_id, utility::Point p, unsigned_id_type free_edge_id);

private:

    std::reference_wrapper<database::DatabaseHelper> d_;

    database::DbGraph* db_graph_;

    EdgeFactory edge_factory_;

    static const size_t kAdjEdgeFrom = 0;
    static const size_t kAdjEdgeTo = 1;
    static const size_t kClosestEdgeFrom = 2;
    static const size_t kClosestEdgeTo = 3;
    static const size_t kLength = 4;
    static const size_t kGeometry = 5;
    static const size_t kSelectedSegmentLength = 6;
    static const size_t kMaxUid = 7;

    class EdgeInputData{
    public:

        EdgeInputData(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length)
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

        double GetLength() const {
            return length_;
        }

        bool GetUndirected() const {
            return false;
        }

    private:
        unsigned_id_type uid_;
        unsigned_id_type from_;
        unsigned_id_type to_;
        double length_;
    };

    /**
     * Determine which segment was used when adjacent edge was looked for in db and return its index (0 or 1).
     *
     * @param rows Rows representing segments.
     * @return Index of the segment that was used when adjacent edge was looked for in db and return its index (0 or 1).
     */
    size_t CalculateSelectedSegmentIndex(std::vector<database::DbRow>& rows);

    /**
     * Create new edges from segment and add them to `result_edges` vector.
     *
     * @param r Db row representing segment.
     * @param result_edges Output value where new edges are added to.
     * @param edge_id Edge id which is free to use as well as the +1 id.
     * @param endpoint_id Node id which is free to use and which is one endpoint of the segment.
     * @param intersection_id Id of original intersection that will serve as the other endpoint.
     */
void SaveEdge(database::DbRow r, std::vector<typename EdgeFactory::Edge>& result_edges, std::vector<std::pair<unsigned_id_type, std::string>>& result_geometries,
    unsigned_id_type endpoint_id, unsigned_id_type intersection_id, unsigned_id_type free_edge_id);
};

template <typename EdgeFactory>
EndpointEdgesCreator<EdgeFactory>::EndpointEdgesCreator(database::DatabaseHelper& d, database::DbGraph* db_graph, const EdgeFactory& edge_factory)
            : d_(d), db_graph_(db_graph), edge_factory_(edge_factory) {}

template <typename EdgeFactory>
std::pair<std::vector<typename EdgeFactory::Edge>, std::vector<std::pair<unsigned_id_type, std::string>>> EndpointEdgesCreator<EdgeFactory>::CalculateEndpointEdges(
        const std::string& table_name, unsigned_id_type endpoint_id, utility::Point p, unsigned_id_type free_edge_id) {
    std::vector<database::DbRow> rows = d_.get().GetClosestSegments(p, table_name, db_graph_);

    // Endpoint has a closest edge that has no neighbours.
    if (rows.size() == 0) {
        throw RouteNotFoundException{"Route cannot be found - endpoint edge has no neighbours."};
    }

    std::vector<typename EdgeFactory::Edge> result_edges{};
    std::vector<std::pair<unsigned_id_type, std::string>> result_geometries{};

    unsigned_id_type selected_segment_index;
    // If closest point to endpoint is an endpoint of the closest edge then
    // there is only one segment.
    // Otherwise find the segment id in db rows that intersects adjacent edge from sql query.
    if (rows.size() > 1) {
        selected_segment_index = CalculateSelectedSegmentIndex(rows);
    } else {
        selected_segment_index = 0;
    }

    // Create edges from the segment that intersects adjacent edge from sql query.
    database::DbRow selected_seg_row = rows[selected_segment_index];
    unsigned_id_type adj_edge_from = selected_seg_row.get<unsigned_id_type>(kAdjEdgeFrom);
    unsigned_id_type adj_edge_to = selected_seg_row.get<unsigned_id_type>(kAdjEdgeTo);
    unsigned_id_type closest_edge_from = selected_seg_row.get<unsigned_id_type>(kClosestEdgeFrom);
    unsigned_id_type closest_edge_to = selected_seg_row.get<unsigned_id_type>(kClosestEdgeTo);

    // Determine the segment's endpoint osm id (it is one of closest edge intersections)
    // The other endpoint is newly created point (split point).
    bool closest_edge_from_used = false;
    unsigned_id_type segment_original_intersection = 0;
    if (adj_edge_from == closest_edge_from) { segment_original_intersection = adj_edge_from; closest_edge_from_used = true; }
    if (adj_edge_from == closest_edge_to) { segment_original_intersection = adj_edge_from; }
    if (adj_edge_to == closest_edge_from) { segment_original_intersection = adj_edge_to; closest_edge_from_used = true; }
    if (adj_edge_to == closest_edge_to) { segment_original_intersection = adj_edge_to; }

    SaveEdge(selected_seg_row, result_edges, result_geometries, endpoint_id, segment_original_intersection, free_edge_id);
    ++free_edge_id;

    if (rows.size() == 1) {
        return std::make_pair(result_edges, result_geometries);
    }

    // Create edges for the other segment.
    size_t other_index = (selected_segment_index == 0) ? 1 : 0;

    unsigned_id_type other_intersection = closest_edge_from_used ? closest_edge_to : closest_edge_from;
    SaveEdge(rows[other_index], result_edges, result_geometries, endpoint_id, other_intersection, free_edge_id);
    ++free_edge_id;
    return std::make_pair(result_edges, result_geometries);
}

template <typename EdgeFactory>
size_t EndpointEdgesCreator<EdgeFactory>::CalculateSelectedSegmentIndex(std::vector<database::DbRow>& rows) {
    double tol = 0.001;

    database::DbRow r0 = rows[0];
    database::DbRow r1 = rows[1];
    size_t selected_segment_index;
    // Check which segment has the same length as the segment that intersects adjacent from sql.
    // If lengths are the same it does not matter which one is picked.
    double length = r0.get<double>(kLength);
    double selected_segment_len = r0.get<double>(kSelectedSegmentLength);
    if ((length - selected_segment_len) * (length - selected_segment_len) < tol) {
        selected_segment_index = 0;
    } else {
        length = r1.get<double>(kLength);
        selected_segment_len = r1.get<double>(kSelectedSegmentLength);
        if ((length - selected_segment_len) * (length - selected_segment_len) < tol) {
            selected_segment_index = 1;
        } else {
//                std::cout << "raise tol!! \n";
            throw RouteNotFoundException{"Endpoint segments script error:No segment has the same length as the previous one."};
        }
    }
    return selected_segment_index;
}

template <typename EdgeFactory>
void EndpointEdgesCreator<EdgeFactory>::SaveEdge(database::DbRow r, std::vector<typename EdgeFactory::Edge>& result_edges, std::vector<std::pair<unsigned_id_type, std::string>>& result_geometries,
    unsigned_id_type endpoint_id, unsigned_id_type intersection_id, unsigned_id_type free_edge_id) {
    double length = r.get<double>(kLength);
    std::string geometry = r.get<std::string>(kGeometry);
    result_edges.push_back(edge_factory_.Create(EdgeInputData{free_edge_id, endpoint_id, intersection_id, length}));
    result_geometries.push_back(std::make_pair(free_edge_id, geometry));
}







}
}
#endif //ROUTING_QUERY_ENPOINT_EDGES_CREATOR_H