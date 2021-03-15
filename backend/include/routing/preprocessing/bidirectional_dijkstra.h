#ifndef BACKEND_BIDIRECTIONAL_DIJKSTRA_H
#define BACKEND_BIDIRECTIONAL_DIJKSTRA_H

#include "routing/edges/basic_edge.h"
#include "routing/algorithm.h"
#include <vector>
#include <set>
#include <queue>
#include <cassert>
#include <limits>

namespace routing {
namespace preprocessing {

template <typename G>
class BidirectionalDijkstra {
public:
    using Vertex = typename G::V;
    using Edge = typename G::E;
    using QueuePair = std::pair<double, Vertex*>;
    

    BidirectionalDijkstra(G & g);

     /**
     * Find the best route from `start_node` to `end_node`.
     *
     * @param start_node Node the routing start from.
     * @param end_node Node the routing ends in.
     * @return Vector of edges that represent the best route.
     */
    void Run(unsigned_id_type start_node, unsigned_id_type end_node);

private:
    /**
     * Graph where dijkstra's algorithm is used.
     */
    G & g_;
    struct PriorityQueueMember;
    struct MinQueueComparator;
    class Direction;
    class ForwardDirection;
    class BackwardDirection;

    using PriorityQueue = std::priority_queue<PriorityQueueMember>;

    PriorityQueueMember GetMin(PriorityQueue& a, PriorityQueue& b);

    
    void UpdateNeighbours(const Vertex& vertex, Vertex& neighbour, PriorityQueue& queue, bool forward_search);

    struct MinQueueComparator {
        bool operator() (const PriorityQueueMember& a , const PriorityQueueMember& b) {
            return a.priority > b.priority;
        }
    };

    struct PriorityQueueMember {
        double priority;
        unsigned_id_type vertex_id;
        Direction direction;

        PriorityQueueMember() : priority(std::numeric_limits<double>::max()), vertex_id(0), direction() {}

        PriorityQueueMember(double p, unsigned_id_type v, const Direction& dir) : priority(p), vertex_id(v), direction(dir) {}


    };

    class Direction {
    protected:
        PriorityQueue queue_;
    public:

        Direction(PriorityQueue* q) : queue_(q) {} 
        virtual ~Direction() {}
        virtual void SetCost(Vertex& vertex, double cost);
        virtual double GetCost(Vertex& vertex);
        virtual void ForEachEdge(Vertex& vertex, std::function<void(Edge&)> f);
        virtual void Enqueue(double priority, unsigned_id_type vertex_id);
    };

    class ForwardDirection : public Direction {
    public:

        ForwardDirection(PriorityQueue* q) : Direction(q) {}
        
        void SetCost(Vertex& vertex, double cost) override {
            vertex.set_forward_cost(cost);
        }

        double GetCost(Vertex& vertex) override {
            return vertex.get_forward_cost();
        }

        void ForEachEdge(Vertex& vertex, std::function<void(Edge&)> f) override {
            vertex.ForEachEdge(f);
        }

        void Enqueue(double priority, unsigned_id_type vertex_id) override {
            this->queue_.emplace(priority, vertex_id, *this);
        }

    };

    class BackwardDirection : public Direction {
    public:

        BackwardDirection(PriorityQueue* q) : Direction(q) {}

        void SetCost(Vertex& vertex, double cost) override {
            vertex.set_backward_cost(cost);
        }

        double GetCost(Vertex& vertex) override {
            return vertex.get_backward_cost();
        }

        void ForEachEdge(Vertex& vertex, std::function<void(Edge&)> f) override {
            vertex.ForEachReverseEdge(f);
        }

        void Enqueue(double priority, unsigned_id_type vertex_id) override {
            this->queue_.emplace(priority, vertex_id, *this);
        }

    };

};

template <typename G>
void BidirectionalDijkstra<G>::Run(unsigned_id_type start_node, unsigned_id_type end_node) {
    PriorityQueue forward_queue;
    PriorityQueue backward_queue;
    forward_queue.emplace(0, start_node, ForwardDirection{&forward_queue});
    backward_queue.emplace(0, end_node, BackwardDirection{&backward_queue});
    
    while (!forward_queue.empty() || !backward_queue.empty()) {
        PriorityQueueMember min_member = GetMin(forward_queue, backward_queue);
        Vertex* vertex = g_.GetVertex(min_member.vertex_id);
        Direction direction = min_member.direction;
        // if vertex->backward_cost, vertex->forward_cost settled - update path length.

        direction.ForEachEdge(*vertex, [&](Edge& edge) {
            Vertex* neighbour = g_.GetVertex(edge.get_to());
            double new_cost = direction.GetCost(*vertex) + edge.get_length();
            if (new_cost < direction.GetCost(*neighbour)) {
                direction.SetCost(*neighbour, new_cost);

            }
        });

    }
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

template <typename G>
void BidirectionalDijkstra<G>::UpdateNeighbours(const Vertex& vertex, Vertex& neighbour, PriorityQueue& queue, bool forward_search) {
    // for(auto&& edge : ((forward_search) ? vertex.get_edges() : vertex.get_reverse_edges())) {
    //     if (forward_search) {
    //         if (neighbour.get_forward_cost() > vertex.get_forward_cost() + edge.get_cost()) {
    //             neighbour.set_forward_cost(vertex.get_forward_cost() + edge.get_cost());
    //             queue.emplace(vertex.get_forward_cost(), vertex.get_osm_id());
    //             neighbour.set_previous()
    //         }
    //     } else {

    //     }
    // }
    
}

}
}

#endif //BACKEND_BIDIRECTIONAL_DIJKSTRA_H
