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

    /**
     * Routing graph which can be used for any edge and vertex types.
     * However, Vertex and Edge must be valid with respect to each other.
     * @tparam Vertex Type of vertex in the graph.
     * @tparam Edge Type of edge in the graph.
     */
    template <typename Vertex, typename Edge>
    class Graph {
        /**
         * Internal graph representaions. Unfortunately, even though the ids
         * are number they are too high to index using std::vector.
         */
        std::unordered_map<unsigned_id_type, Vertex> g_;
    public:

        Graph();

        /**
         * Add edge represented by row to graph.
         * @param row Row representing graph edge.
         */
        void AddEdge(database::EdgeDbRow & row);

        /**
         * Add `edge` to graph.
         * @param edge Edge which is added to graph.
         */
        void AddEdge(Edge&& edge);

        /**
         * Return point to vertex with `id`.
         * @param id Id of vertex to which Vertex* points.
         * @return Pointer to vetex or nullptr if it is not found.
         */
        Vertex* GetVertex(unsigned_id_type id);
    };

    template <typename Vertex, typename Edge>
    Graph<Vertex, Edge>::Graph() : g_(std::unordered_map<unsigned_id_type, Vertex>{}) {}

    template <typename Vertex, typename Edge>
    void Graph<Vertex, Edge>::AddEdge(database::EdgeDbRow & r) {
        AddEdge(std::move(Edge{r}));
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
}

#endif //BACKEND_GRAPH_H
