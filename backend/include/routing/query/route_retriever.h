#ifndef BACKEND_ROUTE_RETRIEVER_H
#define BACKEND_ROUTE_RETRIEVER_H

#include <exception>
#include <string>
#include <functional>
#include <vector>
#include <stack>
#include "routing/edges/basic_edge.h"

namespace routing {
namespace query {

template <typename Graph>
class RouteRetriever {
public:
    using Vertex = Graph::Vertex;
    using Edge = Graph::Edge;

    RouteRetriever(Graph& g);

    class GraphInfo {
    public:
        GraphInfo(RouteRetriever& retriever) : retriever_(retriever) {}

        virtual unsigned_id_type GetPrevious(const Vertex& vertex) = 0;

        virtual Edge& FindEdge(Vertex& vertex, const std::function<bool(const Edge&)>& f) = 0;

        virtual void AddEdge(std::vector<Edge>& route, Edge&& edge) = 0;
    protected:
        RouteRetriever& retriever_;
    };

    class BiDijkstraForwardGraphInfo : public GraphInfo {
        using GraphInfo::retriever_;
    public:
        BiDijkstraForwardGraphInfo(RouteRetriever& retriever) : GraphInfo(retriever) {}

        unsigned_id_type GetPrevious(const Vertex& vertex) override {
            return vertex.get_forward_previous();
        }

        Edge& FindEdge(Vertex& vertex, const std::function<bool(const Edge&)>& f) override {
            return vertex.FindEdge(f);
        }

        void AddEdge(std::vector<Edge>& route, Edge&& edge) override {
            if (edge.IsShortcut()) {
                std::vector<Edge> underlying_edges = retriever_.UnpackShortcut(this, std::move(edge));
                route.insert(route.end(), underlying_edges.begin(), underlying_edges.end());
            } else {
                route.push_back(std::move(edge));
            }
        }
    };

    class BiDijkstraBackwardGraphInfo : public GraphInfo {
        using GraphInfo::retriever_;
    public:
        BiDijkstraBackwardGraphInfo(RouteRetriever& retriever) : GraphInfo(retriever) {}

        unsigned_id_type GetPrevious(const Vertex& vertex) override {
            return vertex.get_backward_previous();
        }

        Edge& FindEdge(Vertex& vertex, const std::function<bool(const Edge&)>& f) override {
            return vertex.FindReverseEdge(f);
        }

        void AddEdge(std::vector<Edge>& route, Edge&& edge) override {
            if (edge.IsShortcut()) {
                std::vector<Edge> underlying_edges = retriever_.UnpackShortcut(this, std::move(edge));
                for(auto&& e : underlying_edges) {
                    e.Reverse();
                }
                route.insert(route.end(), underlying_edges.begin(), underlying_edges.end());
            } else {
                edge.Reverse();
                route.push_back(std::move(edge));
            }
        }
    };

    class DijkstraGraphInfo : public GraphInfo {
    public:
        DijkstraGraphInfo(RouteRetriever& retriever) : GraphInfo(retriever) {}

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
    
    Graph& g_;

    std::vector<Edge> UnpackShortcut(GraphInfo* graph_info, Edge&& shortcut);

    Edge& GetUnderlyingEdge(GraphInfo* graph_info, unsigned_id_type former_vertex_id, unsigned_id_type latter_vertex_id);
};
  
template <typename Graph>
RouteRetriever<Graph>::RouteRetriever(Graph& g) : g_(g) {}

template <typename Graph>
std::vector<typename RouteRetriever<Graph>::Edge> RouteRetriever<Graph>::GetRoute(GraphInfo* graph_info, unsigned_id_type start_node, unsigned_id_type end_node) {
    std::vector<Edge> route;
    if (start_node == end_node) {
        return route;
    }
    unsigned_id_type prev = end_node;
    Vertex& end_vertex = g_.GetVertex(end_node);
    unsigned_id_type cur = graph_info->GetPrevious(end_vertex);
    Vertex& curv = g_.GetVertex(cur);


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

template <typename Graph>
std::vector<typename RouteRetriever<Graph>::Edge> RouteRetriever<Graph>::UnpackShortcut(GraphInfo* graph_info, Edge&& shortcut) {
    assert(shortcut.IsShortcut());
    std::stack<Edge> shortcut_stack{};
    Edge edge = std::move(shortcut);
    std::vector<Edge> underlying_edges{};
    // Reverse in-order traversal of shortcut binary tree - right subtree is first added (due to route reversal later!!)
    while (!shortcut_stack.empty() || edge.IsShortcut()) {
        if (edge.IsShortcut()) {
            shortcut_stack.push(edge);
            edge = GetUnderlyingEdge(graph_info, edge.get_contracted_vertex(), edge.get_to());
        } else {
            // Add the non-shortcut edges to route.
            underlying_edges.push_back(std::move(edge));
            edge = shortcut_stack.top();
            shortcut_stack.pop();

            // The shortcut itself is useless - nothing is done with it.
            edge = GetUnderlyingEdge(graph_info, edge.get_from(), edge.get_contracted_vertex());
        }
    }
    // Last non-shortcut edge was not added due to empty stack and not being a shortcut.
    underlying_edges.push_back(std::move(edge));
   

    return underlying_edges;
}

template <typename Graph>
inline RouteRetriever<Graph>::Edge& RouteRetriever<Graph>::GetUnderlyingEdge(GraphInfo* graph_info, unsigned_id_type former_vertex_id, unsigned_id_type latter_vertex_id) {
    return graph_info->FindEdge(g_.GetVertex(former_vertex_id), [=](const Edge& e) {
        return e.get_to() == latter_vertex_id;
    });

}
    

}
}

#endif //BACKEND_ROUTE_RETRIEVER_H
