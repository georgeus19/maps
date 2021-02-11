#ifndef BACKEND_DIJKSTRA_H
#define BACKEND_DIJKSTRA_H
#include <vector>
#include "routing/edge.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/vertices/contraction_vertex.h"
#include "routing/graph.h"
#include "routing/exception.h"
#include <queue>
#include <algorithm>

namespace routing {

/**
 * Implementation of dijkstra's routing algorithm.
 */
template <typename G>
class Dijkstra {
public:
    using Vertex = typename G::V;
    using Edge = typename G::E;
    using QueuePair = std::pair<double, Vertex*>;

    Dijkstra(G & g);

    /**
     * Find the best route from `start_node` to `end_node`.
     *
     * @param start_node Node the routing start from.
     * @param end_node Node the routing ends in.
     * @return Vector of edges that represent the best route.
     */
    void Run(unsigned_id_type start_node, unsigned_id_type end_node);

     /**
     * Backtrack from `start_node` to `end_node` and find route edges.
     *
     * @param start_node Node which is the start node of backtracking = end node of route.
     * @param end_node Node which is the end node of backtracking = start node of route.
     * @return Vector of edges which represent the found route.
     */
    std::vector<Edge> GetRoute(unsigned_id_type end_node);

    bool Run(unsigned_id_type start_node, std::function<bool(Vertex *)> end_condition);

    double GetPathLength(unsigned_id_type to);
private:
    /**
     * Graph where dijkstra's algorithm is used.
     */
    G & g_;

    unsigned_id_type start_node_;

    void UpdateNeighbours(Vertex * v, std::set<QueuePair> & q);

    void InitGraph();
};

    template< typename G>
    Dijkstra<G>::Dijkstra(G & g) : g_(g), start_node_(0) {}

    template< typename G>
    void Dijkstra<G>::Run(unsigned_id_type start_node, unsigned_id_type end_node) {
        if (!Run(start_node, [=](Dijkstra<G>::Vertex* v) { return v->get_osm_id() == end_node; })) {
            throw RouteNotFoundException("Route from " + std::to_string(start_node) + " to " + std::to_string(end_node) + " could not be found");
        }
    }

    template< typename G>
    std::vector<typename Dijkstra<G>::Edge> Dijkstra<G>::GetRoute(unsigned_id_type end_node) {
        unsigned_id_type prev = end_node;
        Vertex * end_vertex = g_.GetVertex(end_node);
        unsigned_id_type cur = end_vertex->get_previous();
        Vertex * curv = g_.GetVertex(cur);

        std::vector<Dijkstra::Edge> route;

        if (end_vertex->GetPreviousDefaultValue() == end_vertex->get_previous()) {
            return route;
        }

        while (cur != start_node_) {
            Edge& e = curv->FindEdge([=](const Edge& e) {
                return e.get_to() == prev;
            });
            route.push_back(e);

            prev = cur;
            cur = curv->get_previous();
            curv = g_.GetVertex(cur);
        }

        // Find the correct edge of the route's first vertex == start_node_.
        Edge& e = curv->FindEdge([=](const Edge& e) {
            return e.get_to() == prev;
        });
        route.push_back(e);

        std::reverse(route.begin(), route.end());;
        return route; 
    }

    template <typename G>
    bool Dijkstra<G>::Run(unsigned_id_type start_node, std::function<bool(Vertex *)> end_condition) {
        // Priority queue is implemented with Set of pairs of double(=cost) and Vertex*.
        // This is the default possible implementation of std::pair.
        // So pairs with the same cost can be in the set when they belong to different vertices.
        //
        // template <class T1, class T2>
        // bool operator<(const pair<T1, T2>& x, const pair<T1, T2>& y)
        // {
        //     return x.first < y.first ||
        //            (!(y.first < x.first) && x.second < y.second);
        // }
        std::set<QueuePair> q;
        start_node_ = start_node;
        InitGraph();

        Vertex * v = g_.GetVertex(start_node);
        v->set_cost(0);
        q.insert(std::make_pair(v->get_cost(), v));

        while (q.empty() == false) {
            auto&& it = q.begin();
            v = it->second;
            q.erase(it);

            if (end_condition(v)) {
                return true;
            }

            UpdateNeighbours(v, q);
        }
        return false;
    }

    template <typename G>
    double Dijkstra<G>::GetPathLength(unsigned_id_type to) {
        return g_.GetVertex(to)->cost_;
    }

    template <typename G>
    void Dijkstra<G>::UpdateNeighbours(Vertex * v, std::set<QueuePair> & q) {
        v->ForEachEdge([&](Edge & edge) {
            Vertex * neighbour = g_.GetVertex(edge.get_to());
            if (neighbour->get_cost() > v->get_cost() + edge.get_length()) {

                // Only vertices with updated values are in priority queue.
                if (neighbour->get_cost() != std::numeric_limits<double>::max()) {
                    q.erase(std::make_pair(neighbour->get_cost(), neighbour));
                }

                neighbour->set_cost(v->get_cost() + edge.get_length());
                neighbour->set_previous(v->get_osm_id());

                q.insert(std::make_pair(neighbour->get_cost(), neighbour));

            }
        });
    }

    template <typename G>
    void Dijkstra<G>::InitGraph() {
        
        g_.forEachVertex([](Vertex &  v) {
            v.ResetRoutingProperties();
        }); 
    }
    



}
#endif //BACKEND_DIJKSTRA_H