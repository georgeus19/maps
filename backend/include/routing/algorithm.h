#ifndef BACKEND_ALGORITHM_H
#define BACKEND_ALGORITHM_H
#include "routing/edge.h"
#include <vector>
#include "routing/vertex.h"
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
        Algorithm(typename Implementation::G  & graph) : impl_(graph) {}

        /**
         * Find the best route from `start_node` to `end_node`.
         *
         * @param start_node Node the routing start from.
         * @param end_node Node the routing ends in.
         * @return Vector of edges that represent the best route.
         */
        std::vector<typename Implementation::Edge> Run(unsigned_id_type start_node, unsigned_id_type end_node) {
            return impl_.Run(start_node, end_node);
        }

    };

}
#endif //BACKEND_ALGORITHM_H
