#ifndef BACKEND_GRAPH_H
#define BACKEND_GRAPH_H
#include <unordered_map>
#include <vector>
#include <utility>
#include <queue>
#include "routing/edge.h"
#include "database_helper.h"
#include "routing/vertex.h"
#include <set>

namespace routing {
    template <typename Vertex, typename Edge>
    class Graph {
        std::unordered_map<unsigned_id_type, Vertex> g_;
    public:

        Graph();

        void AddEdge(EdgeDbRow);

        Vertex* GetVertex(unsigned_id_type id);

//        std::priority_queue<Vertex*, std::vector<Vertex*>, CostComparer<Vertex*>> GetVertices();

        std::multiset<Vertex*, CostComparer<Vertex*>> GetVertices();

    };

    template <typename Vertex, typename Edge>
    Graph<Vertex, Edge>::Graph() : g_(std::unordered_map<unsigned_id_type, Vertex>{}) {}

    template <typename Vertex, typename Edge>
    void Graph<Vertex, Edge>::AddEdge(EdgeDbRow r) {
        Edge e{
                r.get<unsigned_id_type>(0),
                r.get<std::string>(1),
                r.get<unsigned_id_type>(2),
                r.get<unsigned_id_type>(3),
                r.get<double>(4)
        };

        auto&& it = g_.find(e.get_from());
        if (it == g_.end()) {
            unsigned_id_type from_node = e.get_from();
            // Vertex is not in the graph. Add Vertex with the edge.
            Vertex v{from_node, std::vector<Edge>{}};
            v.AddEdge(std::move(e));
            g_.insert(std::make_pair<unsigned_id_type, Vertex>(std::move(from_node), std::move(v)));
        } else {
            // Vertex exists. Add edge.
            it->second.AddEdge(std::move(e));
        }

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
    std::multiset<Vertex*, CostComparer<Vertex*>> Graph<Vertex, Edge>::GetVertices() {
       std::multiset<Vertex*, CostComparer<Vertex*>> res;
        for(auto&& it = g_.begin(); it != g_.end(); ++it) {
            std::cout << it->second.osm_id_ << " ";
            std::cout << &(it->second) << std::endl;
            res.insert(&(it->second));
//            res.push(&(it->second));
        }
        return res;
    }
}

#endif //BACKEND_GRAPH_H
