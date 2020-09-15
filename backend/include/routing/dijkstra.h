#ifndef BACKEND_DIJKSTRA_H
#define BACKEND_DIJKSTRA_H
#include <vector>
#include "routing/edge.h"
#include "routing/vertex.h"
#include "routing/graph.h"
#include "routing/exception.h"
#include <queue>
#include <algorithm>

namespace routing {

    /**
     * Implementation of dijkstra's routing algorithm.
     */
    class Dijkstra {
    public:
        using Vertex = BasicVertex;
        using Edge = BasicEdge;
        using G = Graph<Vertex, Edge>;

        Dijkstra(G & g);

        /**
         * Find the best route from `start_node` to `end_node`.
         *
         * @param start_node Node the routing start from.
         * @param end_node Node the routing ends in.
         * @return Vector of edges that represent the best route.
         */
        std::vector<Edge> Run(unsigned_id_type start_node, unsigned_id_type end_node);
    private:
        /**
         * Graph where dijkstra's algorithm is used.
         */
        G & g_;

        /**
         * Backtrack from `start_node` to `end_node` and find route edges.
         *
         * @param start_node Node which is the start node of backtracking = end node of route.
         * @param end_node Node which is the end node of backtracking = start node of route.
         * @return Vector of edges which represent the found route.
         */
        std::vector<Edge> CreateRoute(unsigned_id_type start_node, unsigned_id_type end_node);
    };
}
#endif //BACKEND_DIJKSTRA_H