#ifndef BACKEND_GRAPH_H
#define BACKEND_GRAPH_H
#include <unordered_map>
#include <vector>
#include <utility>
#include <queue>
#include "routing/edge.h"
#include "database/database_helper.h"
#include "routing/vertex.h"
#include <set>

namespace routing {
    template <typename Vertex, typename Edge>
    class Graph {
        std::unordered_map<unsigned_id_type, Vertex> g_;
    public:

        Graph();

        void AddEdge(database::EdgeDbRow &);

        void AddEdge(Edge&&);

        Vertex* GetVertex(unsigned_id_type id);

//        std::priority_queue<Vertex*, std::vector<Vertex*>, CostComparer<Vertex*>> GetVertices();

        std::set<std::pair<double, Vertex*>, CostComparer<std::pair<double, Vertex*>>> GetVertices();

    };

    template <typename Vertex, typename Edge>
    Graph<Vertex, Edge>::Graph() : g_(std::unordered_map<unsigned_id_type, Vertex>{}) {}

    template <typename Vertex, typename Edge>
    void Graph<Vertex, Edge>::AddEdge(database::EdgeDbRow & r) {

        AddEdge(std::move(Edge{r}));
/*
        // Add edge to->from, this is a temporary measure...
        {
            Edge e{
                    r.get<unsigned_id_type>(0),
                    r.get<std::string>(1),
                    r.get<unsigned_id_type>(3),
                    r.get<unsigned_id_type>(2),
                    r.get<double>(4)
            };
            auto&& it = g_.find(e.get_from());
            if (it == g_.end()) {
                unsigned_id_type from_node = e.get_from();
                Vertex v{from_node, std::vector<Edge>{}};
                v.AddEdge(std::move(e));
                g_.insert(std::make_pair<unsigned_id_type, Vertex>(std::move(from_node), std::move(v)));
            } else {
                it->second.AddEdge(std::move(e));
            }
        }
        */
    }

    template <typename Vertex, typename Edge>
    void Graph<Vertex, Edge>::AddEdge(Edge && e) {
        unsigned_id_type from_node = e.get_from();
        unsigned_id_type to_node = e.get_to();
        auto&& from_it = g_.find(from_node);
        if (from_it == g_.end()) {
            unsigned_id_type from_node = e.get_from();
            // Vertex is not in the graph. Add Vertex with the edge.
            Vertex v{from_node, std::vector<Edge>{}};
            v.AddEdge(e);
            g_.insert(std::make_pair<unsigned_id_type, Vertex>(std::move(from_node), std::move(v)));
        } else {
            // Vertex exists. Add edge.
            (from_it->second).AddEdge(std::move(e));
        }

        // e.to_ might not be added to the graph.
        // So try add it with no outgoing edges.
        auto && to_it = g_.find(to_node);
        if (to_it == g_.end()) {
            Vertex to_vertex{to_node, std::vector<Edge>{}};
            g_.insert(std::make_pair<unsigned_id_type, Vertex>(std::move(to_node), std::move(to_vertex)));
        }
    }


    template <typename Vertex, typename Edge>
    inline Vertex* Graph<Vertex, Edge>::GetVertex(unsigned_id_type id) {
        auto&& it = g_.find(id);
        if (it == g_.end()) {
            return nullptr;
        } else {
            return &((it->second));
        }
    }

//    template <typename Vertex, typename Edge>
//    std::priority_queue<Vertex*, std::vector<Vertex*>, CostComparer<Vertex*>> Graph<Vertex, Edge>::GetVertices() {
//        std::priority_queue<Vertex*, std::vector<Vertex*>, CostComparer<Vertex*>> res;
//        for(auto&& it = g_.begin(); it != g_.end(); ++it) {
//            std::cout << it->second.osm_id_ << " ";
//            std::cout << &(it->second) << std::endl;
//            res.push(&(it->second));
//        }
//        return res;
//    }

    template <typename Vertex, typename Edge>
    std::set<std::pair<double, Vertex*>, CostComparer<std::pair<double, Vertex*>>> Graph<Vertex, Edge>::GetVertices() {
       std::set<std::pair<double, Vertex*>, CostComparer<std::pair<double, Vertex*>>> res;
        for(auto&& it = g_.begin(); it != g_.end(); ++it) {
            res.insert(std::make_pair<double, Vertex*>((it->second).cost_, &(it->second)));
        }
        return res;
    }
}

#endif //BACKEND_GRAPH_H
