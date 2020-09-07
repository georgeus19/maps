#ifndef BACKEND_DIJKSTRA_H
#define BACKEND_DIJKSTRA_H
#include <vector>
#include "routing/edge.h"
#include "routing/vertex.h"
#include "routing/graph.h"
#include <queue>
#include <algorithm>

namespace routing {

    class Dijkstra {
    public:
        using Vertex = BasicVertex;
        using Edge = BasicEdge;
        using G = Graph<Vertex, Edge>;

        Dijkstra(G & g);

        std::vector<Edge> Run(unsigned_id_type start_node, unsigned_id_type end_node);
    private:
        G & g_;

        std::vector<Edge> CreateRoute(unsigned_id_type start_node, unsigned_id_type end_node);
    };
}
#endif //BACKEND_DIJKSTRA_H
