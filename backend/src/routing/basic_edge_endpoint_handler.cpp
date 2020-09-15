#include "routing/basic_edge_endpoint_handler.h"
using namespace database;
using namespace std;

namespace routing {

    BasicEdgeEndpointHandler::BasicEdgeEndpointHandler(unsigned_id_type node_id_from, unsigned_id_type node_id_to,
                                                       unsigned_id_type edge_id_from, unsigned_id_type edge_id_to)
            : segment_geometries_(GeomMap{}), node_id_from_(node_id_from), node_id_to_(node_id_to),
              edge_id_from_(edge_id_from), edge_id_to_(edge_id_to) {}

    std::vector<BasicEdge> BasicEdgeEndpointHandler::CalculateEndpointEdges(utility::Point p, const std::string &table_name, database::DatabaseHelper &d) {
        vector<DbRow> rows = d.GetClosestSegments(p, table_name);

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

        vector<BasicEdge> result_edges{};

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

        DbRow selected_seg_row = rows[selected_segment_index];
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

    std::string BasicEdgeEndpointHandler::GetEndpointEdgeGeometry(unsigned_id_type edge_id) {
        auto&& it = segment_geometries_.find(edge_id);
        if (it == segment_geometries_.end()) {
            return string{};
        } else {
            return it->second;
        }
    }


    unsigned_id_type BasicEdgeEndpointHandler::get_node_id_to() {
        return node_id_to_;
    }

    unsigned_id_type BasicEdgeEndpointHandler::get_edge_id_to() {
        return edge_id_to_;
    }

    size_t BasicEdgeEndpointHandler::CalculateSelectedSegmentIndex(vector<DbRow> & rows) {
        double tol = 0.001;

        DbRow r0 = rows[0];
        DbRow r1 = rows[1];
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

    void BasicEdgeEndpointHandler::AddReciprocalEdges(DbRow r, vector<BasicEdge> & result_edges, unsigned_id_type & edge_id,
                                                      unsigned_id_type node_id, unsigned_id_type intersection_id) {
        double length = r.get<double>(kLength);
        std::string geometry = r.get<string>(kGeometry);

        segment_geometries_.insert(make_pair(edge_id, geometry));
        result_edges.push_back(BasicEdge{edge_id++, node_id, intersection_id, length});
        segment_geometries_.insert(make_pair(edge_id, geometry));
        result_edges.push_back(BasicEdge{edge_id++, intersection_id, node_id, length});

    }
}