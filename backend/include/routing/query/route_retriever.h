#ifndef BACKEND_ROUTE_RETRIEVER_H
#define BACKEND_ROUTE_RETRIEVER_H

#include <exception>
#include <string>
#include <functional>
#include <vector>
#include "routing/edges/basic_edge.h"


namespace routing {

template <typename G>
class RouteRetriever {
public:
    using Vertex = G::V;
    using Edge = G::E;

    RouteRetriever(G& g);

    class GraphInfo {
    public:
        GraphInfo() {}

        virtual unsigned_id_type GetPrevious(const Vertex& vertex) = 0;
    };

    class BiDijkstraForwardGraphInfo {
    public:
        BiDijkstraForwardGraphInfo() {

        }
        unsigned_id_type GetPrevious(const Vertex& vertex) override {
            return vertex.get_forward_previous();
        }
    };

    class BiDijkstraBackwardGraphInfo {
    public:
        BiDijkstraBackwardGraphInfo() {}

        unsigned_id_type GetPrevious(const Vertex& vertex) override {
            return vertex.get_backward_previous();
        }
    };

    class DijkstraGraphInfo : public GraphInfo {
    public:
        DijkstraGraphInfo() {}

        unsigned_id_type GetPrevious(const Vertex& vertex) override {
            return vertex.get_previous();
        }
    };

    std::vector<Edge> GetRoute(GraphInfo* graph_info, unsigned_id_type start_node, unsigned_id_type end_node);

private:
    
    
    G& g_;
    

};
  
template <typename G>
RouteRetriever<G>::RouteRetriever(G& g) : g_(g) {}

template <typename G>
std::vector<typename RouteRetriever<G>::Edge> RouteRetriever<G>::GetRoute(GraphInfo* graph_info, unsigned_id_type start_node, unsigned_id_type end_node) {
    unsigned_id_type prev = end_node;
    Vertex * end_vertex = g_.GetVertex(end_node);
    unsigned_id_type cur = graph_info->GetPrevious(*end_vertex); // end_vertex->get_previous();
    Vertex * curv = g_.GetVertex(cur);

    std::vector<Edge> route;

    if (end_vertex->GetPreviousDefaultValue() == graph_info->GetPrevious(*end_vertex) /* end_vertex->get_previous() */ ) {
        return route;
    }

    while (cur != start_node) {
        Edge& e = curv->FindEdge([=](const Edge& e) {
            return e.get_to() == prev;
        });
        route.push_back(e);

        prev = cur;
        cur = graph_info->GetPrevious(*curv); // curv->get_previous();
        curv = g_.GetVertex(cur);
    }

    // Find the correct edge of the route's first vertex == start_node.
    Edge& e = curv->FindEdge([=](const Edge& e) {
        return e.get_to() == prev;
    });
    route.push_back(e);

    std::reverse(route.begin(), route.end());;
    return route; 
}



}

#endif //BACKEND_ROUTE_RETRIEVER_H
