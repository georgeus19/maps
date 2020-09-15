#ifndef BACKEND_BASIC_EDGE_ENDPOINT_HANDLER_H
#define BACKEND_BASIC_EDGE_ENDPOINT_HANDLER_H
#include <unordered_map>
#include <string>
#include "routing/edge.h"
#include "database/database_helper.h"
#include "routing/exception.h"
#include "routing/endpoint_handler.h"
namespace routing {

    class BasicEdgeEndpointHandler {
        using GeomMap = std::unordered_map<unsigned_id_type, std::string>;
        GeomMap segment_geometries_;
        unsigned_id_type node_id_from_;
        unsigned_id_type node_id_to_;
        unsigned_id_type edge_id_from_;
        unsigned_id_type edge_id_to_;

        const size_t kAdjEdgeFrom = 0;
        const size_t kAdjEdgeTo = 1;
        const size_t kClosestEdgeFrom = 2;
        const size_t kClosestEdgeTo = 3;
        const size_t kLength = 4;
        const size_t kGeometry = 5;
        const size_t kSelectedSegmentLength = 6;
        const size_t kMaxUid = 7;
    public:

        using Edge = BasicEdge;

        BasicEdgeEndpointHandler(unsigned_id_type node_id_from, unsigned_id_type node_id_to,
                                 unsigned_id_type edge_id_from, unsigned_id_type edge_id_to);

        std::vector<BasicEdge> CalculateEndpointEdges(utility::Point p, const std::string & table_name, database::DatabaseHelper & d);

        std::string GetEndpointEdgeGeometry(unsigned_id_type edge_id);

        unsigned_id_type get_node_id_to();

        unsigned_id_type get_edge_id_to();
    private:
        size_t CalculateSelectedSegmentIndex(std::vector<database::DbRow> & rows);

        void AddReciprocalEdges(database::DbRow r, std::vector<BasicEdge> & result_edges, unsigned_id_type & edge_id,
                                                          unsigned_id_type node_id, unsigned_id_type intersection_id);
    };
}

#endif //BACKEND_BASIC_EDGE_ENDPOINT_HANDLER_H
