#include "routing/dijkstra.h"
using namespace std;
namespace routing {

    Dijkstra::Dijkstra(G & g) : g_(g) {}

    std::vector<Dijkstra::Edge> Dijkstra::Run(unsigned_id_type start_node, unsigned_id_type end_node) {
        Vertex * v = g_.GetVertex(start_node);
        v->cost_ = 0;
        multiset<Vertex*, CostComparer<Vertex*>> q = move(g_.GetVertices());

        while (q.empty() == false) {
            auto&& it = q.begin();
            v = *it;
            q.erase(it);
//            auto&& node_handler = q.extract(0);
//            v = node_handler.value();
//                v = q.top();
//            q.pop();
            if (v->osm_id_ == end_node) {
                return Dijkstra::CreateRoute(end_node, start_node);
            }
            for (auto&& edge: v->outgoing_edges_) {
                Vertex * neighbour = g_.GetVertex(edge.get_to());
                if (neighbour->cost_ > v->cost_ + edge.length_) {
                    neighbour->cost_ = v->cost_ + edge.length_;
                    neighbour->previous_ = v->osm_id_;
                    q.erase(neighbour);
                    q.insert(neighbour);

                }
            }
        }
        throw 1;
    }

    std::vector<Dijkstra::Edge> Dijkstra::CreateRoute(unsigned_id_type start_node, unsigned_id_type end_node) {
        unsigned_id_type cur = start_node;
        unsigned_id_type prev = start_node;
        Vertex * curv = g_.GetVertex(cur);
        cur = curv->previous_;
        std::vector<Dijkstra::Edge> route;
        while (cur != end_node) {

            for(auto&& e : curv->outgoing_edges_) {
                if (e.get_to() == prev) {
                    route.push_back(e);
                    break;
                }
            }

            curv = g_.GetVertex(cur);
            prev = cur;
            cur = curv->previous_;
        }
        return route;
    }
}

