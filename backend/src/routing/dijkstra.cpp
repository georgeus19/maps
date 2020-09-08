#include "routing/dijkstra.h"
using namespace std;
namespace routing {

    Dijkstra::Dijkstra(G & g) : g_(g) {}

    std::vector<Dijkstra::Edge> Dijkstra::Run(unsigned_id_type start_node, unsigned_id_type end_node) {
        // Init priority queue.
        using QueuePair = pair<double, Vertex*>;
//        template <class T1, class T2>
//        bool operator<(const pair<T1, T2>& x, const pair<T1, T2>& y)
//        {
//            return x.first < y.first ||
//                   (!(y.first < x.first) && x.second < y.second);
//        }
        set<QueuePair> q;// = move(g_.GetVertices());

        Vertex * v = g_.GetVertex(start_node);
        v->cost_ = 0;
        q.insert(make_pair(v->cost_, v));

        while (q.empty() == false) {
            auto&& it = q.begin();
            v = it->second;
            q.erase(it);

//            if (v->osm_id_ == end_node) {
//                return Dijkstra::CreateRoute(end_node, start_node);
//            }
            for (auto&& edge: v->outgoing_edges_) {
                Vertex * neighbour = g_.GetVertex(edge.get_to());
                if (neighbour->cost_ > v->cost_ + edge.length_) {

                    // Only vertices with updated values are in priority queue.
                    if (neighbour->cost_ != std::numeric_limits<double>::max()) {
                        q.erase(make_pair(neighbour->cost_, neighbour));
                    }

                    neighbour->cost_ = v->cost_ + edge.length_;
                    neighbour->previous_ = v->osm_id_;

                    q.insert(make_pair(neighbour->cost_, neighbour));

                }
            }
        }
        return Dijkstra::CreateRoute(end_node, start_node);
    }

    std::vector<Dijkstra::Edge> Dijkstra::CreateRoute(unsigned_id_type start_node, unsigned_id_type end_node) {
        unsigned_id_type prev = start_node;
        unsigned_id_type cur = g_.GetVertex(start_node)->previous_;
        Vertex * curv = g_.GetVertex(cur);

        std::vector<Dijkstra::Edge> route;
        while (cur != end_node) {

            for(auto&& e : curv->outgoing_edges_) {
                if (e.get_to() == prev) {
                    route.push_back(e);
                    break;
                }
            }

            prev = cur;
            cur = curv->previous_;
            curv = g_.GetVertex(cur);
        }

        for(auto&& e : curv->outgoing_edges_) {
            if (e.get_to() == prev) {
                route.push_back(e);
                break;
            }
        }

        return route;
    }
}
