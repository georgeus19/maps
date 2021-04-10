#ifndef BACKEND_BASIC_EDGE_ENDPOINT_HANDLER_H
#define BACKEND_BASIC_EDGE_ENDPOINT_HANDLER_H
#include <unordered_map>
#include <string>
#include "routing/edges/basic_edge.h"
#include "database/database_helper.h"
#include "database/db_graph.h"
#include "routing/exception.h"
#include "routing/endpoint_handler.h"
namespace routing {

/**
 * BasicEdgeEndpointHandler handles creating edges from endpoint to closest intersection - Splitting
 * the closest edge to multiple segments so that routing is done more accurately.
 *
 * It also stores the segments' geometries which can be later queried for to construct
 * the geometry of the whole route.
 *
 * This is the implementation of EndpointHandler for graph with BasicEdge edges.
 */
template <typename Edge>
class BasicEdgeEndpointHandler {
    using GeomMap = std::unordered_map<unsigned_id_type, std::string>;
    /**
     * Stored geometries of created segments.
     */
    GeomMap segment_geometries_;

    database::DbGraph* db_graph_;

    /**
     * Number from which greater numbers are free to use as new node ids.
     */
    unsigned_id_type node_id_from_;

    /**
     * Number from which greater numbers are free to use as new node ids.
     */
    unsigned_id_type node_id_to_;

    /**
     * Number from which greater numbers are free to use as new edge ids.
     */
    unsigned_id_type edge_id_from_;

    /**
     * Number from which greater numbers are free to use as new edge ids.
     */
    unsigned_id_type edge_id_to_;

    /**
     * Indices to rows acquired from db when looking for segments.
     */
    const size_t kAdjEdgeFrom = 0;
    const size_t kAdjEdgeTo = 1;
    const size_t kClosestEdgeFrom = 2;
    const size_t kClosestEdgeTo = 3;
    const size_t kLength = 4;
    const size_t kGeometry = 5;
    const size_t kSelectedSegmentLength = 6;
    const size_t kMaxUid = 7;
public:

    BasicEdgeEndpointHandler(database::DbGraph* db_graph, unsigned_id_type node_id_from, unsigned_id_type node_id_to,
                                unsigned_id_type edge_id_from, unsigned_id_type edge_id_to);

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
    std::vector<Edge> CalculateEndpointEdges(utility::Point p, const std::string & table_name, database::DatabaseHelper & d);

    /**
     * Return previously saved segments geometries.
     *
     * @param edge_id Segment (now edge) id.
     * @return Previously saved segment geometries.
     */
    std::string GetEndpointEdgeGeometry(unsigned_id_type edge_id);

    unsigned_id_type get_node_id_to();

    unsigned_id_type get_edge_id_to();
private:
    /**
     * Determine which segment was used when adjacent edge was looked for in db and return its index (0 or 1).
     *
     * @param rows Rows representing segments.
     * @return Index of the segment that was used when adjacent edge was looked for in db and return its index (0 or 1).
     */
    size_t CalculateSelectedSegmentIndex(std::vector<database::DbRow> & rows);

    /**
     * Create new edges from segment and add them to `result_edges` vector.
     *
     * @param r Db row representing segment.
     * @param result_edges Output value where new edges are added to.
     * @param edge_id Edge id which is free to use as well as the +1 id.
     * @param node_id Node id which is free to use and which is one endpoint of the segment.
     * @param intersection_id Id of original intersection that will serve as the other endpoint.
     */
    void AddReciprocalEdges(database::DbRow r, std::vector<Edge> & result_edges, unsigned_id_type & edge_id,
                                                        unsigned_id_type node_id, unsigned_id_type intersection_id);
};

template <typename Edge>
BasicEdgeEndpointHandler<Edge>::BasicEdgeEndpointHandler(database::DbGraph* db_graph, unsigned_id_type node_id_from, unsigned_id_type node_id_to,
                                                       unsigned_id_type edge_id_from, unsigned_id_type edge_id_to)
            : segment_geometries_(), db_graph_(db_graph), node_id_from_(node_id_from), node_id_to_(node_id_to),
              edge_id_from_(edge_id_from), edge_id_to_(edge_id_to) {}

template <typename Edge>
std::vector<Edge> BasicEdgeEndpointHandler<Edge>::CalculateEndpointEdges(utility::Point p, const std::string &table_name, database::DatabaseHelper &d) {
    std::vector<database::DbRow> rows = d.GetClosestSegments(p, table_name, db_graph_);

    // Endpoint has a closest edge that has no neighbours.
    if (rows.size() == 0) {
        throw RouteNotFoundException{"Route cannot be found - endpoint edge has no neighbours."};
    }

    // When user does not know which edge ids are free and endpoint handler has not been used before.
    if (edge_id_from_ == edge_id_to_) {
        edge_id_from_ = rows[0].get<unsigned_id_type>(kMaxUid) + 1;
    }
    // When user does not know which node ids are free and endpoint handler has not been used before.
    if (node_id_from_ == node_id_to_) {
        node_id_from_ = 0;
    }
    unsigned_id_type edge_id = edge_id_from_;
    unsigned_id_type node_id = node_id_from_;

    std::vector<Edge> result_edges{};

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

    AddReciprocalEdges(selected_seg_row, result_edges, edge_id, node_id, segment_original_intersection);

    if (rows.size() == 1) {
        return result_edges;
    }

    // Create edges for the other segment.

    size_t other_index = (selected_segment_index == 0) ? 1 : 0;

    unsigned_id_type other_intersection = closest_edge_from_used ? closest_edge_to : closest_edge_from;
    AddReciprocalEdges(rows[other_index], result_edges, edge_id, node_id, other_intersection);

    ++node_id;
    edge_id_to_ = edge_id;
    node_id_to_ = node_id;

    return result_edges;
}

template <typename Edge>
std::string BasicEdgeEndpointHandler<Edge>::GetEndpointEdgeGeometry(unsigned_id_type edge_id) {
    auto&& it = segment_geometries_.find(edge_id);
    if (it == segment_geometries_.end()) {
        return std::string{};
    } else {
        return it->second;
    }
}

template <typename Edge>
unsigned_id_type BasicEdgeEndpointHandler<Edge>::get_node_id_to() {
    return node_id_to_;
}

template <typename Edge>
unsigned_id_type BasicEdgeEndpointHandler<Edge>::get_edge_id_to() {
    return edge_id_to_;
}

template <typename Edge>
size_t BasicEdgeEndpointHandler<Edge>::CalculateSelectedSegmentIndex(std::vector<database::DbRow> & rows) {
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

template <typename Edge>
void BasicEdgeEndpointHandler<Edge>::AddReciprocalEdges(database::DbRow r, std::vector<Edge> & result_edges, unsigned_id_type & edge_id,
                                                    unsigned_id_type node_id, unsigned_id_type intersection_id) {
    double length = r.get<double>(kLength);
    std::string geometry = r.get<std::string>(kGeometry);

    segment_geometries_.insert(make_pair(edge_id, geometry));
    result_edges.push_back(Edge{edge_id++, node_id, intersection_id, length});
    segment_geometries_.insert(make_pair(edge_id, geometry));
    result_edges.push_back(Edge{edge_id++, intersection_id, node_id, length});

}







}

#endif //BACKEND_BASIC_EDGE_ENDPOINT_HANDLER_H
