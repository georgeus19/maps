#ifndef BACKEND_BIDIRECTIONAL_DIJKSTRA_H
#define BACKEND_BIDIRECTIONAL_DIJKSTRA_H

#include "routing/edges/basic_edge.h"
#include "routing/algorithm.h"
#include "routing/query/route_retriever.h"
#include "tsl/robin_map.h"
#include <vector>
#include <set>
#include <queue>
#include <cassert>
#include <limits>
#include <memory>

namespace routing {
namespace query {

template <typename G>
class BidirectionalDijkstra {
public:
    using Vertex = typename G::Vertex;
    using Edge = typename G::Edge;
    using QueuePair = std::pair<double, Vertex*>;
    using Graph = G;

    BidirectionalDijkstra(G& g);

    /**
     * Find the best route from `start_node` to `end_node`.
     *
     * @param start_node Node the routing start from.
     * @param end_node Node the routing ends in.
     * @return Vector of edges that represent the best route.
     */
    void Run(unsigned_id_type start_node, unsigned_id_type end_node);

    /**
     * Get shortest route from the node the algorithm was run to.
     * @return Vector of edges which represent the found route. Empty if no path found.
     */
    std::vector<Edge> GetRoute();

private:
    struct VertexRoutingProperties;
    struct PriorityQueueMember;
    struct MinQueueComparator;
    class Direction;
    class ForwardDirection;
    class BackwardDirection;
    
    /**
     * Graph where dijkstra's algorithm is used.
     */
    G & g_;
    using UnorderedMap = tsl::robin_map<unsigned_id_type, VertexRoutingProperties>;
    UnorderedMap forward_touched_vertices_;
    UnorderedMap backward_touched_vertices_;
    unsigned_id_type settled_vertex_;
    unsigned_id_type start_node_;
    unsigned_id_type end_node_;

    using PriorityQueue = std::priority_queue<PriorityQueueMember, std::vector<PriorityQueueMember>, MinQueueComparator>;

    PriorityQueueMember GetMin(PriorityQueue& a, PriorityQueue& b);

    double GetSummedCosts(double forward_cost, double backward_cost);

    double GetMaxCost() const;

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
    
    struct MinQueueComparator {
        bool operator() (const PriorityQueueMember& a , const PriorityQueueMember& b) {
            return a.cost_priority > b.cost_priority;
        }
    };

    struct PriorityQueueMember {
        double cost_priority;
        unsigned_id_type vertex_id;
        Direction* direction;

        PriorityQueueMember() : cost_priority(std::numeric_limits<double>::max()), vertex_id(0), direction(nullptr) {}

        PriorityQueueMember(double c, unsigned_id_type v, Direction* dir) : cost_priority(c), vertex_id(v), direction(dir) {}

        bool operator< (const PriorityQueueMember& other) {
            return cost_priority < other.cost_priority;
        }

        bool operator> (const PriorityQueueMember& other) {
            return cost_priority > other.cost_priority;
        }
    };

    class Direction {
    public:
        Direction(PriorityQueue& q, UnorderedMap& tv) : queue_(q), touched_vertices_(tv) {}
        virtual ~Direction() = default;

        void SetRoutingProperties(unsigned_id_type vertex_id, double cost, unsigned_id_type previous) {
            touched_vertices_.insert_or_assign(vertex_id, VertexRoutingProperties{cost, previous});
        }

        VertexRoutingProperties& GetRoutingProperties(unsigned_id_type vertex_id) {
            return touched_vertices_[vertex_id];
        }

        virtual void ForEachEdge(Vertex& vertex, const std::function<void(Edge&)>& f) = 0;
        virtual void Enqueue(double cost_priority, unsigned_id_type vertex_id) = 0;
        virtual unsigned_id_type GetTo(const Edge& e) const = 0;
    protected:
        PriorityQueue& queue_;
        UnorderedMap& touched_vertices_;
    };

    class ForwardDirection : public Direction {
        using Direction::queue_;
    public:

        ForwardDirection(PriorityQueue& q, UnorderedMap& tv) : Direction(q, tv) {}

        void ForEachEdge(Vertex& vertex, const std::function<void(Edge&)>& f) override {
            vertex.ForEachEdge(f);
        }

        void Enqueue(double cost_priority, unsigned_id_type vertex_id) override {
            queue_.emplace(cost_priority, vertex_id, this);
        }

        unsigned_id_type GetTo(const Edge& e) const override {
            return e.get_to();
        }
    };

    class BackwardDirection : public Direction {
        using Direction::queue_;
    public:

        BackwardDirection(PriorityQueue& q, UnorderedMap& tv) : Direction(q, tv) {}

        void ForEachEdge(Vertex& vertex, const std::function<void(Edge&)>& f) override {
            vertex.ForEachBackwardEdge(f);
        }

        void Enqueue(double cost_priority, unsigned_id_type vertex_id) override {
            queue_.emplace(cost_priority, vertex_id, this);
        }

        unsigned_id_type GetTo(const Edge& e) const override {
            return e.get_backward_to();
        }
    };
};

template <typename G>
BidirectionalDijkstra<G>::BidirectionalDijkstra(G & g) : g_(g), forward_touched_vertices_(), backward_touched_vertices_(), settled_vertex_(0) {}

template <typename G>
void BidirectionalDijkstra<G>::Run(unsigned_id_type start_node, unsigned_id_type end_node) {
    start_node_ = start_node;
    end_node_ = end_node;
    forward_touched_vertices_.clear();
    backward_touched_vertices_.clear();
    PriorityQueue forward_queue;
    PriorityQueue backward_queue;
    ForwardDirection forward_direction{forward_queue, forward_touched_vertices_};
    BackwardDirection backward_direction{backward_queue, backward_touched_vertices_};
    forward_queue.emplace(0, start_node, &forward_direction);
    backward_queue.emplace(0, end_node, &backward_direction);

    forward_touched_vertices_.insert_or_assign(start_node, VertexRoutingProperties{0, 0});
    backward_touched_vertices_.insert_or_assign(end_node, VertexRoutingProperties{0, 0});

    double min_path_length = std::numeric_limits<double>::max();
    
    while (!forward_queue.empty() || !backward_queue.empty()) {
        PriorityQueueMember min_member = GetMin(forward_queue, backward_queue);
        Vertex& vertex = g_.GetVertex(min_member.vertex_id);
        Direction* direction = min_member.direction;
        VertexRoutingProperties vertex_routing_properties = direction->GetRoutingProperties(vertex.get_osm_id());
        bool queue_member_is_dead = vertex_routing_properties.cost < min_member.cost_priority;
        if (queue_member_is_dead) {
            continue; // The element already has lower cost - dead element in queue (was added multiple time with diff costs).
        }
        double path_length = GetSummedCosts(forward_touched_vertices_[vertex.get_osm_id()].cost, backward_touched_vertices_[vertex.get_osm_id()].cost);
        if (path_length < min_path_length) {
            min_path_length = path_length;
            settled_vertex_ = vertex.get_osm_id();
        }
        // if vertex->backward_cost, vertex->forward_cost settled - update path length.

        direction->ForEachEdge(vertex, [&](Edge& edge) {
            unsigned_id_type neighbour_id = direction->GetTo(edge);
            Vertex& neighbour = g_.GetVertex(neighbour_id);
            double new_cost = vertex_routing_properties.cost + edge.get_length();
            VertexRoutingProperties& neighbour_routing_properties = direction->GetRoutingProperties(neighbour_id);
            if (vertex.get_ordering_rank() < neighbour.get_ordering_rank() && new_cost < neighbour_routing_properties.cost) {
                neighbour_routing_properties.cost = new_cost;
                neighbour_routing_properties.previous = vertex.get_osm_id();
                direction->Enqueue(new_cost, neighbour_id);
            }
        });

    }

    if (min_path_length == std::numeric_limits<double>::max()) {
        throw RouteNotFoundException("Route from " + std::to_string(start_node) + " to " + std::to_string(end_node) + " could not be found");
    }

}

template <typename G>
std::vector<typename BidirectionalDijkstra<G>::Edge> BidirectionalDijkstra<G>::GetRoute() {
    RouteRetriever<G, UnorderedMap> r{g_};
    typename RouteRetriever<G, UnorderedMap>::BiDijkstraForwardGraphInfo forward_routing_info{r, forward_touched_vertices_};
    typename RouteRetriever<G, UnorderedMap>::BiDijkstraBackwardGraphInfo backward_routing_info{r, backward_touched_vertices_};
    auto&& forward_route = r.GetRoute(&forward_routing_info, start_node_, settled_vertex_);
    auto&& backward_route = r.GetRoute(&backward_routing_info, end_node_, settled_vertex_);
    forward_route.insert(forward_route.end(), backward_route.rbegin(), backward_route.rend());
    return forward_route;
}

template <typename G>
typename BidirectionalDijkstra<G>::PriorityQueueMember BidirectionalDijkstra<G>::GetMin(PriorityQueue& a, PriorityQueue& b) {
    PriorityQueueMember atop = ((!a.empty()) ? a.top() : PriorityQueueMember{} );
    PriorityQueueMember btop = ((!b.empty()) ? b.top() : PriorityQueueMember{} );
    if (atop.cost_priority < btop.cost_priority) {
        a.pop();
        return atop;
    } else {
        b.pop();
        return btop;
    }
}

template <typename G>
double BidirectionalDijkstra<G>::GetSummedCosts(double forward_cost, double backward_cost) {
    double max = std::max(forward_cost, backward_cost);
    if (max != GetMaxCost()) { 
        return forward_cost + backward_cost;
    } else {
        return max;
    }
}

template <typename G>
inline double BidirectionalDijkstra<G>:: GetMaxCost() const {
    return std::numeric_limits<double>::max();
}


}
}

#endif //BACKEND_BIDIRECTIONAL_DIJKSTRA_H
