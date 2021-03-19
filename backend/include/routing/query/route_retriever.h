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

        virtual Edge& FindEdge(Vertex& vertex, const std::function<bool(const Edge&)>& f) = 0;

        virtual void AddEdge(std::vector<Edge>& route, Edge&& edge) = 0;
    };

    class BiDijkstraForwardGraphInfo : public GraphInfo {
    public:
        BiDijkstraForwardGraphInfo() {}

        unsigned_id_type GetPrevious(const Vertex& vertex) override {
            return vertex.get_forward_previous();
        }

        Edge& FindEdge(Vertex& vertex, const std::function<bool(const Edge&)>& f) override {
            return vertex.FindEdge(f);
        }

        void AddEdge(std::vector<Edge>& route, Edge&& edge) override {
            route.push_back(std::move(edge));
        }
    };

    class BiDijkstraBackwardGraphInfo : public GraphInfo {
    public:
        BiDijkstraBackwardGraphInfo() {}

        unsigned_id_type GetPrevious(const Vertex& vertex) override {
            return vertex.get_backward_previous();
        }

        Edge& FindEdge(Vertex& vertex, const std::function<bool(const Edge&)>& f) override {
            return vertex.FindReverseEdge(f);
        }

        void AddEdge(std::vector<Edge>& route, Edge&& edge) override {
            edge.Reverse();
            route.push_back(std::move(edge));
        }
    };

    class DijkstraGraphInfo : public GraphInfo {
    public:
        DijkstraGraphInfo() {}

        unsigned_id_type GetPrevious(const Vertex& vertex) override {
            return vertex.get_previous();
        }

        Edge& FindEdge(Vertex& vertex, const std::function<bool(const Edge&)>& f) override {
            return vertex.FindEdge(f);
        }

        void AddEdge(std::vector<Edge>& route, Edge&& edge) override {
            route.push_back(std::move(edge));
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
    Vertex& end_vertex = g_.GetVertex(end_node);
    unsigned_id_type cur = graph_info->GetPrevious(end_vertex);
    Vertex& curv = g_.GetVertex(cur);

    std::vector<Edge> route;

    if (end_vertex.GetPreviousDefaultValue() == graph_info->GetPrevious(end_vertex) ) {
        return route;
    }

    while (cur != start_node) {
        Edge edge = graph_info->FindEdge(curv, [=](const Edge& e) {
            return e.get_to() == prev;
        });
        graph_info->AddEdge(route, std::move(edge));

        prev = cur;
        cur = graph_info->GetPrevious(curv);
        curv = g_.GetVertex(cur);
    }

    // Find the correct edge of the route's first vertex == start_node.
    Edge edge = graph_info->FindEdge(curv, [=](const Edge& e) {
        return e.get_to() == prev;
    });
    graph_info->AddEdge(route, std::move(edge));

    std::reverse(route.begin(), route.end());;
    return route; 
}



}

#endif //BACKEND_ROUTE_RETRIEVER_H
