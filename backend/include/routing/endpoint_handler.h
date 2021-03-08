#ifndef BACKEND_ENDPOINT_HANDLER_H
#define BACKEND_ENDPOINT_HANDLER_H
#include <unordered_map>
#include <string>
#include "routing/edges/basic_edge.h"
#include "database/database_helper.h"


namespace routing {

    /**
     * EndpointHandler handles creating edges from endpoint to closest intersection - Splitting
     * the closest edge to multiple segments so that routing is done more accurately.
     *
     * It also stores the segments' geometries which can be later queried for to construct
     * the geometry of the whole route.
     *
     * EndpointHandler is without implementation and all function calls are forwarder to EndpointHandlerImpl class.
     * @tparam EndpointHandlerImpl Implementation class which function calls are forwarded to.
     */
    template <typename EndpointHandlerImpl>
    class EndpointHandler {
        EndpointHandlerImpl impl_;
    public:
        EndpointHandler(unsigned_id_type node_id_from, unsigned_id_type node_id_to,
                unsigned_id_type edge_id_from, unsigned_id_type edge_id_to) : impl_(EndpointHandlerImpl{node_id_from, node_id_to, edge_id_from, edge_id_to}) {
        }

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
        std::vector<typename EndpointHandlerImpl::Edge> CalculateEndpointEdges(utility::Point p, const std::string &table_name, database::DatabaseHelper &d) {
            return impl_.CalculateEndpointEdges(p, table_name, d);
        }

        /**
         * Return previously saved segments geometries.
         *
         * @param edge_id Segment (now edge) id.
         * @return Previously saved segment geometries.
         */
        std::string GetEndpointEdgeGeometry(unsigned_id_type edge_id) {
            return impl_.GetEndpointEdgeGeometry(edge_id);
        }

        unsigned_id_type get_node_id_to() {
            return impl_.get_node_id_to();
        }

        unsigned_id_type get_edge_id_to() {
            return impl_.get_edge_id_to();
        }
    };

}

#endif //BACKEND_ENDPOINT_HANDLER_H
