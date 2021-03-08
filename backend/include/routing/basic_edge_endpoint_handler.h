#ifndef BACKEND_BASIC_EDGE_ENDPOINT_HANDLER_H
#define BACKEND_BASIC_EDGE_ENDPOINT_HANDLER_H
#include <unordered_map>
#include <string>
#include "routing/edges/basic_edge.h"
#include "database/database_helper.h"
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
    class BasicEdgeEndpointHandler {
        using GeomMap = std::unordered_map<unsigned_id_type, std::string>;
        /**
         * Stored geometries of created segments.
         */
        GeomMap segment_geometries_;

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

        using Edge = BasicEdge;

        BasicEdgeEndpointHandler(unsigned_id_type node_id_from, unsigned_id_type node_id_to,
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
        std::vector<BasicEdge> CalculateEndpointEdges(utility::Point p, const std::string & table_name, database::DatabaseHelper & d);

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
        void AddReciprocalEdges(database::DbRow r, std::vector<BasicEdge> & result_edges, unsigned_id_type & edge_id,
                                                          unsigned_id_type node_id, unsigned_id_type intersection_id);
    };
}

#endif //BACKEND_BASIC_EDGE_ENDPOINT_HANDLER_H
