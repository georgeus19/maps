#ifndef BACKEND_BIDIRECTIONAL_DIJKSTRA_H
#define BACKEND_BIDIRECTIONAL_DIJKSTRA_H

#include "routing/edges/basic_edge.h"
#include "routing/algorithm.h"
#include "routing/query/route_retriever.h"
#include <vector>
#include <set>
#include <queue>
#include <cassert>
#include <limits>
#include <memory>

namespace routing {
namespace preprocessing {

template <typename G>
class BidirectionalDijkstra {
public:
    using Vertex = typename G::V;
    using Edge = typename G::E;
    using QueuePair = std::pair<double, Vertex*>;

    friend class RouteRetriever;
    

    BidirectionalDijkstra(G & g);

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
    /**
     * Graph where dijkstra's algorithm is used.
     */
    G & g_;
    unsigned_id_type settled_vertex_;
    unsigned_id_type start_node_;
    unsigned_id_type end_node_;
    struct PriorityQueueMember;
    struct MinQueueComparator;
    class Direction;
    class ForwardDirection;
    class BackwardDirection;

    using PriorityQueue = std::priority_queue<PriorityQueueMember, std::vector<PriorityQueueMember>, MinQueueComparator>;

    PriorityQueueMember GetMin(PriorityQueue& a, PriorityQueue& b);

    
    struct MinQueueComparator {
        bool operator() (const PriorityQueueMember& a , const PriorityQueueMember& b) {
            return a.priority > b.priority;
        }
    };

    struct PriorityQueueMember {
        double priority;
        unsigned_id_type vertex_id;
        Direction* direction;

        PriorityQueueMember() : priority(std::numeric_limits<double>::max()), vertex_id(0), direction(nullptr) {}

        PriorityQueueMember(double p, unsigned_id_type v, Direction* dir) : priority(p), vertex_id(v), direction(dir) {}

        bool operator< (const PriorityQueueMember& other) {
            return priority < other.priority;
        }

        bool operator> (const PriorityQueueMember& other) {
            return priority > other.priority;
        }


    };

    class Direction {
    protected:
        PriorityQueue* queue_;
    public:

        Direction(PriorityQueue* q) : queue_(q) {}
        Direction() : queue_() {}  
        virtual ~Direction() {}
        virtual void SetCost(Vertex& vertex, double cost) = 0;
        virtual double GetCost(Vertex& vertex) = 0;
        virtual void SetPrevious(Vertex& vertex, unsigned_id_type previous) = 0;
        virtual unsigned_id_type GetPrevious(Vertex& vertex) = 0;
        virtual void ForEachEdge(Vertex& vertex, std::function<void(Edge&)> f) = 0;
        virtual void Enqueue(double priority, unsigned_id_type vertex_id) = 0;
    };

    class ForwardDirection : public Direction {
        using Direction::queue_;
    public:

        ForwardDirection(PriorityQueue* q) : Direction(q) {}
        ForwardDirection() : Direction() {}
        
        void SetCost(Vertex& vertex, double cost) override {
            vertex.set_forward_cost(cost);
        }

        double GetCost(Vertex& vertex) override {
            return vertex.get_forward_cost();
        }

        void SetPrevious(Vertex& vertex, unsigned_id_type previous) override {
            vertex.set_forward_previous(previous);
        }
        
        unsigned_id_type GetPrevious(Vertex& vertex) override {
            return vertex.get_forward_previous();
        }

        void ForEachEdge(Vertex& vertex, std::function<void(Edge&)> f) override {
            vertex.ForEachEdge(f);
        }

        void Enqueue(double priority, unsigned_id_type vertex_id) override {
            queue_->emplace(priority, vertex_id, this);
        }

    };

    class BackwardDirection : public Direction {
        using Direction::queue_;
    public:

        BackwardDirection(PriorityQueue* q) : Direction(q) {}
        BackwardDirection() : Direction() {}

        void SetCost(Vertex& vertex, double cost) override {
            vertex.set_backward_cost(cost);
        }

        double GetCost(Vertex& vertex) override {
            return vertex.get_backward_cost();
        }

        void SetPrevious(Vertex& vertex, unsigned_id_type previous) override {
            vertex.set_backward_previous(previous);
        }
        
        unsigned_id_type GetPrevious(Vertex& vertex) override {
            return vertex.get_backward_previous();
        }

        void ForEachEdge(Vertex& vertex, std::function<void(Edge&)> f) override {
            vertex.ForEachReverseEdge(f);
        }

        void Enqueue(double priority, unsigned_id_type vertex_id) override {
            queue_->emplace(priority, vertex_id, this);
        }

    };

};

template <typename G>
BidirectionalDijkstra<G>::BidirectionalDijkstra(G & g) : g_(g), settled_vertex_(0) {}

template <typename G>
void BidirectionalDijkstra<G>::Run(unsigned_id_type start_node, unsigned_id_type end_node) {
    start_node_ = start_node;
    end_node_ = end_node;
    PriorityQueue forward_queue;
    PriorityQueue backward_queue;
    ForwardDirection forward_direction{&forward_queue};
    BackwardDirection backward_direction{&backward_queue};
    forward_queue.emplace(0, start_node, &forward_direction);
    backward_queue.emplace(0, end_node, &backward_direction);
    g_.GetVertex(start_node)->set_forward_cost(0);
    g_.GetVertex(end_node)->set_backward_cost(0);

    double min_path_length = std::numeric_limits<double>::max();
    
    while (!forward_queue.empty() || !backward_queue.empty()) {
        PriorityQueueMember min_member = GetMin(forward_queue, backward_queue);
        Vertex* vertex = g_.GetVertex(min_member.vertex_id);
        Direction* direction = min_member.direction;
        double path_length = vertex->GetSummedCosts();
        if (path_length < min_path_length) {
            min_path_length = path_length;
            settled_vertex_ = vertex->get_osm_id();
        }
        // if vertex->backward_cost, vertex->forward_cost settled - update path length.

        direction->ForEachEdge(*vertex, [&, vertex, direction](Edge& edge) {
            Vertex* neighbour = g_.GetVertex(edge.get_to());
            double new_cost = direction->GetCost(*vertex) + edge.get_length();
            if (vertex->get_order_id() < neighbour->get_order_id() && new_cost < direction->GetCost(*neighbour)) {
                direction->SetCost(*neighbour, new_cost);
                direction->SetPrevious(*neighbour, vertex->get_osm_id());

            }
        });

    }

    if (min_path_length != std::numeric_limits<double>::max()) {
        throw RouteNotFoundException("Route from " + std::to_string(start_node) + " to " + std::to_string(end_node) + " could not be found");
    }

}

template <typename G>
std::vector<typename BidirectionalDijkstra<G>::Edge> BidirectionalDijkstra<G>::GetRoute() {
    typename RouteRetriever<G>::BiDijkstraForwardGraphInfo forward_graph_info{};
    typename RouteRetriever<G>::BiDijkstraBackwardGraphInfo backward_graph_info{};
    RouteRetriever<G> r{g_};
    std::vector<Edge> forward_route = r.GetRoute(&forward_graph_info, start_node_, settled_vertex_);
    std::vector<Edge> backward_route = r.GetRoute(&backward_graph_info, end_node_, settled_vertex_);
    forward_route.insert(forward_route.end(), backward_route.begin(), backward_route.end());
    return forward_route;
}

template <typename G>
typename BidirectionalDijkstra<G>::PriorityQueueMember BidirectionalDijkstra<G>::GetMin(PriorityQueue& a, PriorityQueue& b) {
    PriorityQueueMember atop = ((!a.empty()) ? a.top() : PriorityQueueMember{} );
    PriorityQueueMember btop = ((!b.empty()) ? b.top() : PriorityQueueMember{} );
    if (atop.priority < btop.priority) {
        a.pop();
        return atop;
    } else {
        b.pop();
        return btop;
    }
}




}
}

#endif //BACKEND_BIDIRECTIONAL_DIJKSTRA_H
