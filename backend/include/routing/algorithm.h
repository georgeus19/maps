#ifndef BACKEND_ALGORITHM_H
#define BACKEND_ALGORITHM_H
#include "routing/edges/basic_edge.h"
#include <vector>
#include "routing/vertices/basic_vertex.h"
#include "routing/vertices/contraction_vertex.h"
                                   
namespace routing {

    /**
     * Class Algorithm provides routing functionality. It forwards all calls to Implementation class.
     *
     * @tparam Implementation Implementation of routing algorithm that all methods are forwarded to.
     */
    template <typename Implementation>
    class Algorithm {
        /**
         * Class with implementation of routing algorithm.
         */
        Implementation impl_;
    public:
        /**
         * Forward `graph` to implementation class as reference.
         *
         * @param graph Graph where routing happens.
         */
        Algorithm(typename Implementation::Graph& graph) : impl_(graph) {}

        /**
         * Find the best route from `start_node` to `end_node`.
         *
         * @param start_node Node the routing start from.
         * @param end_node Node the routing ends in.
         * @return Vector of edges that represent the best route.
         */
        void Run(unsigned_id_type start_node, unsigned_id_type end_node) {
            impl_.Run(start_node, end_node);
        }

         /**
         * Get shortest route from the node the algorithm was run to.
         * @param end_node Endpoint of the route.
         * @return Vector of edges which represent the found route. Empty if no path found.
         */
        std::vector<typename Implementation::Edge> GetRoute() {
            return impl_.GetRoute();
        }

    };

}
#endif //BACKEND_ALGORITHM_H
