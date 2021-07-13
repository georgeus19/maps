#ifndef ROUTING_QUERY_ROUTE_RETRIEVER_H
#define ROUTING_QUERY_ROUTE_RETRIEVER_H

#include "routing/edges/basic_edge.h"
#include "routing/types.h"

#include "tsl/robin_map.h"

#include <exception>
#include <string>
#include <functional>
#include <vector>
#include <stack>

namespace routing {
namespace query {

template <typename Graph, typename VertexRoutingProperties>
class RouteRetriever {
public:
    using Vertex = Graph::Vertex;
    using Edge = Graph::Edge;

    RouteRetriever(Graph& g);

    class GraphInfo {
    public:
        GraphInfo(RouteRetriever& retriever, VertexRoutingProperties& tv) : retriever_(retriever), touched_vertices_(tv) {}

        unsigned_id_type GetPrevious(const Vertex& vertex) const {
            return touched_vertices_[vertex.get_osm_id()].previous;
        }

        virtual Edge& FindEdge(Vertex& vertex, unsigned_id_type target_vertex_id) = 0;

        virtual Edge& FindBackwardEdge(Vertex& vertex, unsigned_id_type target_vertex_id) = 0;

        virtual void AddEdge(std::vector<Edge>& route, Edge&& edge) = 0;
    protected:
        RouteRetriever& retriever_;
        VertexRoutingProperties& touched_vertices_;
    };

    class BiDijkstraForwardGraphInfo : public GraphInfo {
        using GraphInfo::retriever_;
    public:
        BiDijkstraForwardGraphInfo(RouteRetriever& retriever, VertexRoutingProperties& tv) : GraphInfo(retriever, tv) {}

        Edge& FindEdge(Vertex& vertex, unsigned_id_type target_vertex_id) override {
            return vertex.FindEdge([=](const Edge& e) {
                return e.get_to() == target_vertex_id;
            });
        }

        Edge& FindBackwardEdge(Vertex& vertex, unsigned_id_type target_vertex_id) override {
            return vertex.FindBackwardEdge([=](const Edge& e) {
                return e.get_to() == target_vertex_id;
            });
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
        BiDijkstraBackwardGraphInfo(RouteRetriever& retriever, VertexRoutingProperties& tv) : GraphInfo(retriever, tv) {}

        Edge& FindEdge(Vertex& vertex, unsigned_id_type target_vertex_id) override {
            return vertex.FindBackwardEdge([=](const Edge& e) {
                return e.get_to() == target_vertex_id;
            });
        }

        Edge& FindBackwardEdge(Vertex& vertex, unsigned_id_type target_vertex_id) override {
            return vertex.FindEdge([=](const Edge& e) {
                return e.get_to() == target_vertex_id;
            });
        }

        void AddEdge(std::vector<Edge>& route, Edge&& edge) override {
            if (edge.IsShortcut()) {
                std::vector<Edge> underlying_edges = retriever_.UnpackShortcut(this, std::move(edge));
                route.insert(route.end(), underlying_edges.begin(), underlying_edges.end());
            } else {
                edge.Reverse();
                route.push_back(std::move(edge));
            }
        }
    };

    class DijkstraGraphInfo : public GraphInfo {
    public:
        DijkstraGraphInfo(RouteRetriever& retriever, VertexRoutingProperties& tv) : GraphInfo(retriever, tv) {}

        Edge& FindEdge(Vertex& vertex, unsigned_id_type target_vertex_id) override {
            return vertex.FindEdge([=](const Edge& e) {
                return e.get_to() == target_vertex_id;
            });
        }

        Edge& FindBackwardEdge(Vertex& vertex, unsigned_id_type target_vertex_id) override {
            throw NotImplementedException{"DijkstraGraphInfo FindBackwardEdge not implemented - no other reverse edges!"};
        }

        void AddEdge(std::vector<Edge>& route, Edge&& edge) override {
            route.push_back(std::move(edge));
        }
    };

    std::vector<Edge> GetRoute(GraphInfo* graph_info, unsigned_id_type start_node, unsigned_id_type end_node);

private:
    
    Graph& g_;

    std::vector<Edge> UnpackShortcut(GraphInfo* graph_info, Edge&& shortcut);

    Edge& GetUnderlyingEdge(GraphInfo* graph_info, unsigned_id_type source_vertex_id, unsigned_id_type target_vertex_id, bool normal_edge);

    unsigned_id_type GetPreviousDefaultValue() const;
};
  
template <typename Graph, typename VertexRoutingProperties>
RouteRetriever<Graph, VertexRoutingProperties>::RouteRetriever(Graph& g) : g_(g) {}

template <typename Graph, typename VertexRoutingProperties>
std::vector<typename RouteRetriever<Graph, VertexRoutingProperties>::Edge> RouteRetriever<Graph, VertexRoutingProperties>::GetRoute(GraphInfo* graph_info, unsigned_id_type start_node, unsigned_id_type end_node) {
    std::vector<Edge> route;
    if (start_node == end_node) {
        return route;
    }
    unsigned_id_type previous_vertex_id = end_node;
    Vertex& end_vertex = g_.GetVertex(end_node);
    unsigned_id_type current_vertex_id = graph_info->GetPrevious(end_vertex);

    if (GetPreviousDefaultValue() == graph_info->GetPrevious(end_vertex) ) {
        return route;
    }
    while (current_vertex_id != start_node) {
        Vertex& current_vertex = g_.GetVertex(current_vertex_id);
        Edge edge = graph_info->FindEdge(current_vertex, previous_vertex_id);
        graph_info->AddEdge(route, std::move(edge));
        previous_vertex_id = current_vertex_id;
        current_vertex_id = graph_info->GetPrevious(current_vertex);
    }

    // Find the correct edge of the route's first vertex == start_node.
    Edge edge = graph_info->FindEdge(g_.GetVertex(current_vertex_id), previous_vertex_id);
    graph_info->AddEdge(route, std::move(edge));

    std::reverse(route.begin(), route.end());;
    return route; 
}

template <typename Graph, typename VertexRoutingProperties>
std::vector<typename RouteRetriever<Graph, VertexRoutingProperties>::Edge> RouteRetriever<Graph, VertexRoutingProperties>::UnpackShortcut(GraphInfo* graph_info, Edge&& shortcut) {
    assert(shortcut.IsShortcut());
    std::stack<Edge> shortcut_stack{};
    Edge edge = std::move(shortcut);
    std::vector<Edge> underlying_edges{};
    // Reverse in-order traversal of shortcut binary tree - right subtree is first added (due to route reversal later!!)
    while (!shortcut_stack.empty() || edge.IsShortcut()) {
        if (edge.IsShortcut()) {
            shortcut_stack.push(edge);
            edge = GetUnderlyingEdge(graph_info, edge.get_contracted_vertex(), edge.get_to(), true);
        } else {
            // Add the non-shortcut edges to route.
            underlying_edges.push_back(std::move(edge));
            edge = shortcut_stack.top();
            shortcut_stack.pop();

            // The shortcut itself is useless - nothing is done with it.
            
            // The former edge belongs to the downward graph if for forward dijkstra search
            // resp to the upward graph in case of backward dijkstra search.
            // So the edge needs to be reverse so that its direction is correct.
            edge = GetUnderlyingEdge(graph_info, edge.get_from(), edge.get_contracted_vertex(), false);
            edge.Reverse();
        }
    }
    // Last non-shortcut edge was not added due to empty stack and not being a shortcut.
    underlying_edges.push_back(std::move(edge));
   

    return underlying_edges;
}

template <typename Graph, typename VertexRoutingProperties>
inline RouteRetriever<Graph, VertexRoutingProperties>::Edge& RouteRetriever<Graph, VertexRoutingProperties>::GetUnderlyingEdge(
    GraphInfo* graph_info, unsigned_id_type source_vertex_id, unsigned_id_type target_vertex_id, bool normal_edge) {
    if (normal_edge) {
        return graph_info->FindEdge(g_.GetVertex(source_vertex_id), target_vertex_id);
    } else {
        // The other direction -> target is source and the other way around.
        return graph_info->FindBackwardEdge(g_.GetVertex(target_vertex_id), source_vertex_id);
    }
}

template <typename Graph, typename VertexRoutingProperties>
inline unsigned_id_type RouteRetriever<Graph, VertexRoutingProperties>::GetPreviousDefaultValue() const {
    return 0;
}
    

}
}

#endif //ROUTING_QUERY_ROUTE_RETRIEVER_H
