#ifndef BACKEND_ENDPOINT_HANDLER_H
#define BACKEND_ENDPOINT_HANDLER_H
#include <unordered_map>
#include <string>
#include "routing/edge.h"
#include "database/database_helper.h"


namespace routing {

    template <typename EndpointHandlerImpl>
    class EndpointHandler {
        EndpointHandlerImpl impl_;
    public:
        EndpointHandler(unsigned_id_type node_id_from, unsigned_id_type node_id_to,
                unsigned_id_type edge_id_from, unsigned_id_type edge_id_to) : impl_(EndpointHandlerImpl{node_id_from, node_id_to, edge_id_from, edge_id_to}) {
        }

        std::vector<typename EndpointHandlerImpl::Edge> CalculateEndpointEdges(utility::Point p, const std::string &table_name, database::DatabaseHelper &d) {
            return impl_.CalculateEndpointEdges(p, table_name, d);
        }

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
