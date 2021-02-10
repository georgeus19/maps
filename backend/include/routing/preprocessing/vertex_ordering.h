#ifndef BACKEND_NODE_ORDERING_H
#define BACKEND_NODE_ORDERING_H

namespace routing {
namespace preprocessing {

template <typename Graph>
class VertexOrdering {
    Graph & g_;
public:

    VertexOrdering(Graph & g) : g_(g) {}

    typename Graph::V & GetMinVertex();

};

}
}

#endif //BACKEND_NODE_ORDERING_H
