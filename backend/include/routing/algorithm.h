#ifndef BACKEND_ALGORITHM_H
#define BACKEND_ALGORITHM_H
#include "routing/edge.h"
#include <vector>
#include "routing/vertex.h"
namespace routing {

    template <typename Implementation>
    class Algorithm {

        Implementation impl_;
    public:
        Algorithm(typename Implementation::G  & graph) : impl_(graph) {
            //impl_ = Implementation{graph};
        }

        std::vector<typename Implementation::Edge> Run(unsigned_id_type start_node, unsigned_id_type end_node) {
            return impl_.Run(start_node, end_node);
        }

    };

}
#endif //BACKEND_ALGORITHM_H
