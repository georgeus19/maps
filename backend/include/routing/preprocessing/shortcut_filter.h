#ifndef BACKEND_SHORTCUT_FILTER_H
#define BACKEND_SHORTCUT_FILTER_H

#include "routing/edges/basic_edge.h"
#include "routing/preprocessing/ch_dijkstra.h"
#include <vector>
#include <iterator>
#include <set>
#include <queue>
#include <cassert>
#include "routing/preprocessing/contraction_parameters.h"
#include "routing/preprocessing/shortcut_container.h"

namespace routing {
namespace preprocessing {

template <typename Graph>
class ShortcutFilter {
    using Vertex = Graph::V;
    using Edge = Graph::E;
public:
    ShortcutFilter(Graph& g);

    std::vector<Edge> FilterDuplicateEdges(const std::vector<Edge>& edges);

    ShortcutContainer<Edge> ClassifyShortcuts(std::vector<Edge>&& shortcuts);

private:
    Graph& g_;

};


template <typename Graph>
ShortcutFilter<Graph>::ShortcutFilter(Graph& g) : g_(g) {}

template <typename Graph>
std::vector<typename ShortcutFilter<Graph>::Edge> ShortcutFilter<Graph>::FilterDuplicateEdges(const std::vector<Edge>& edges) {
    std::vector<Edge> unique_edges;
    unique_edges.reserve(edges.size());
    for(auto&& a : edges) {
        bool add = true;
        for(auto&& b : edges) {
            bool same_origin = a.get_from() == b.get_from();
            bool same_destination = a.get_to() == b.get_to();
            bool not_the_same_edge = a.get_uid() != b.get_uid(); 
            
            if (same_origin && same_destination && not_the_same_edge) {
                if (a.get_length() > b.get_length()) {
                    add = false;
                } else if (a.get_length() == b.get_length()) {
                    if (a.get_uid() > b.get_uid()) {
                        add = false;
                    }
                }
            }
        }
        if (add) {
            unique_edges.push_back(a);
        }
    }
    return unique_edges;
}

template <typename Graph>
ShortcutContainer<typename ShortcutFilter<Graph>::Edge> ShortcutFilter<Graph>::ClassifyShortcuts(std::vector<Edge>&& shortcuts) {
    std::vector<Edge> new_edges;
    std::vector<Edge> improve_edges;
    for(auto&& shortcut : shortcuts) {
        auto&& source_vertex = g_.GetVertex(shortcut.get_from());
        bool new_edge = true;
        bool improve = false;
        for(auto&& edge : source_vertex.get_edges()) {
            bool same_target = shortcut.get_to() == edge.get_to();
            if (same_target) {
                if (shortcut.get_length() < edge.get_length()) {
                    improve = true;
                } 
                new_edge = false;
            }
        }
        if (new_edge) {
            new_edges.push_back(std::move(shortcut));
        }
        if (improve) {
            improve_edges.push_back(std::move(shortcut));
        }

    }
    return ShortcutContainer<Edge>{std::move(new_edges), std::move(improve_edges)};
}

}
}

#endif //BACKEND_SHORTCUT_FILTER_H