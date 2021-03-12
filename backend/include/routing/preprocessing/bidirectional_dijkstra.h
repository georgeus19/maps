#ifndef BACKEND_BIDIRECTIONAL_DIJKSTRA_H
#define BACKEND_BIDIRECTIONAL_DIJKSTRA_H

#include "routing/edges/basic_edge.h"
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
        bool forward_search;

        PriorityQueueMember() : priority(std::numeric_limits<double>::max()), vertex_id(0), forward_search(true) {}

        PriorityQueueMember(double p, unsigned_id_type v, bool fs) : priority(p), vertex_id(v), forward_search(fs) {}


    };

    class Direction {

    };

    class ForwardDirection {

    };

};

template <typename G>
void BidirectionalDijkstra<G>::Run(unsigned_id_type start_node, unsigned_id_type end_node) {
    PriorityQueue forward_queue;
    PriorityQueue backward_queue;
    forward_queue.emplace(0, start_node);
    backward_queue.emplace(0, end_node);
    
    while (!forward_queue.empty() || !backward_queue.empty()) {
        PriorityQueueMember min_member = GetMin(forward_queue, backward_queue);
        Vertex* vertex = g_.GetVertex(min_member.vertex_id);
        // if vertex->backward_cost, vertex->forward_cost settled - update path length.



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
    for(auto&& edge : ((forward_search) ? vertex.get_edges() : vertex.get_reverse_edges())) {
        if (forward_search) {
            // if (neighbour.get_forward_cost() > vertex.get_forward_cost())
        } else {

        }
    }
    
}

}
}

#endif //BACKEND_BIDIRECTIONAL_DIJKSTRA_H
