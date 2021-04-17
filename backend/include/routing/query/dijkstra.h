#ifndef BACKEND_DIJKSTRA_H
#define BACKEND_DIJKSTRA_H
#include <vector>
#include "routing/edges/basic_edge.h"
#include "routing/vertices/basic_vertex.h"
#include "routing/vertices/contraction_vertex.h"
#include "routing/exception.h"
#include <queue>
#include <algorithm>
#include "routing/query/route_retriever.h"
#include "tsl/robin_map.h"

namespace routing {
namespace query {

/**
 * Implementation of dijkstra's routing algorithm.
 */
template <typename G>
class Dijkstra {
public:
    using Vertex = typename G::Vertex;
    using Edge = typename G::Edge;
    using QueuePair = std::pair<double, Vertex*>;
    using Graph = G;

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

    std::vector<Edge> GetRoute();

    bool Run(unsigned_id_type start_node, const std::function<bool(Vertex *)>& end_condition, const std::function<bool(Vertex*)>& ignore);

    double GetPathLength(unsigned_id_type to);
private:
    struct VertexRoutingProperties;
    /**
     * Graph where dijkstra's algorithm is used.
     */
    G & g_;
    using UnorderedMap = tsl::robin_map<unsigned_id_type, VertexRoutingProperties>;
    UnorderedMap touched_vertices_;

    unsigned_id_type start_node_;
    unsigned_id_type end_node_;

    /**
	 * Stores information from the search. It is memory & performance inefficient to store it in vertices
	 * since a small portion of graph is searched.
	 */
    struct VertexRoutingProperties {
        double cost;
        unsigned_id_type previous;

		VertexRoutingProperties() : cost(std::numeric_limits<double>::max()), previous(0) {}

        VertexRoutingProperties(double c, unsigned_id_type p) : cost(c), previous(p) {}

        VertexRoutingProperties(const VertexRoutingProperties& other) = default;
        VertexRoutingProperties(VertexRoutingProperties&& other) = default;
        VertexRoutingProperties& operator= (const VertexRoutingProperties& other) = default;
        VertexRoutingProperties& operator= (VertexRoutingProperties&& other) = default;
        ~VertexRoutingProperties() = default;
    };

    void UpdateNeighbours(Vertex& v, VertexRoutingProperties& vertex_properties, std::set<QueuePair> & q, const std::function<bool(Vertex*)>& ignore);
};

template <typename G>
Dijkstra<G>::Dijkstra(G & g) : g_(g), touched_vertices_(), start_node_(0), end_node_(0) {}

template <typename G>
void Dijkstra<G>::Run(unsigned_id_type start_node, unsigned_id_type end_node) {
    start_node_ = start_node;
    end_node_ = end_node;
    if (!Run(start_node, [=](Dijkstra<G>::Vertex* v) { return v->get_osm_id() == end_node; }, [](Dijkstra<G>::Vertex*) { return false; })) {
        throw RouteNotFoundException("Route from " + std::to_string(start_node) + " to " + std::to_string(end_node) + " could not be found");
    }
}

template <typename G>
inline std::vector<typename Dijkstra<G>::Edge> Dijkstra<G>::GetRoute(unsigned_id_type end_node) {
    RouteRetriever<G, UnorderedMap> r{g_};
    typename RouteRetriever<G, UnorderedMap>::DijkstraGraphInfo graph_info{r, touched_vertices_};
    return r.GetRoute(&graph_info, start_node_, end_node);
}

template <typename G>
inline std::vector<typename Dijkstra<G>::Edge> Dijkstra<G>::GetRoute() {
    return GetRoute(end_node_);
}

template <typename G>
bool Dijkstra<G>::Run(unsigned_id_type start_node, const std::function<bool(Vertex *)>& end_condition, const std::function<bool(Vertex*)>& ignore) {
    // Priority queue is implemented with Set of pairs of double(=cost) and Vertex*.
    // This is the default possible implementation of std::pair.
    // So pairs with the same cost can be in the set when they belong to different vertices.
    /*
    template <class T1, class T2>
    bool operator<(const pair<T1, T2>& x, const pair<T1, T2>& y)
    {
        return x.first < y.first ||
               (!(y.first < x.first) && x.second < y.second);
    }
    */
    std::set<QueuePair> q;
    start_node_ = start_node;
    touched_vertices_.clear();

    Vertex& start_vertex = g_.GetVertex(start_node);
    touched_vertices_.insert_or_assign(start_node, VertexRoutingProperties{0, 0});
    q.insert(std::make_pair(0, &start_vertex));

    while (q.empty() == false) {
        auto&& it = q.begin();
        Vertex& v = *(it->second);
        q.erase(it);

        if (end_condition(&v)) {
            return true;
        }

        UpdateNeighbours(v, touched_vertices_[v.get_osm_id()], q, ignore);
    }
    return false;
}

template <typename G>
double Dijkstra<G>::GetPathLength(unsigned_id_type to) {
    return touched_vertices_[to].cost;
}

template <typename G>
void Dijkstra<G>::UpdateNeighbours(Vertex& v, VertexRoutingProperties& vertex_properties, std::set<QueuePair>& q, const std::function<bool(Vertex*)>& ignore) {
    v.ForEachEdge([&](Edge & edge) {
        unsigned_id_type neighbour_id = edge.get_to();
        Vertex& neighbour = g_.GetVertex(neighbour_id);
        auto&& neighbour_properties = touched_vertices_[neighbour_id];
        double new_cost = vertex_properties.cost + edge.get_length();
        if (!ignore(&neighbour) && neighbour_properties.cost > new_cost) {

            // Only vertices with updated values are in priority queue.
            if (neighbour_properties.cost != std::numeric_limits<double>::max()) {
                q.erase(std::make_pair(neighbour_properties.cost, &neighbour));
            }
            neighbour_properties.cost = new_cost;
            neighbour_properties.previous = v.get_osm_id();
            q.insert(std::make_pair(neighbour_properties.cost, &neighbour));
        }
    });
}


    



}
}
#endif //BACKEND_DIJKSTRA_H