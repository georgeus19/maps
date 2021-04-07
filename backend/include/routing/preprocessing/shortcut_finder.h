#ifndef BACKEND_SHORTCUT_FINDER_H
#define BACKEND_SHORTCUT_FINDER_H

#include "routing/edges/basic_edge.h"
#include "routing/preprocessing/ch_dijkstra.h"
#include <vector>
#include <iterator>
#include <set>
#include <queue>
#include <cassert>
#include "routing/preprocessing/contraction_parameters.h"
#include "tsl/robin_set.h"
#include "tsl/robin_map.h"
#include "routing/preprocessing/shortcut_container.h"
#include "routing/preprocessing/shortcut_filter.h"

namespace routing {
namespace preprocessing {

template <typename Graph>
class ShortcutFinder {
    using Vertex = Graph::Vertex;
    using Edge = Graph::Edge;
public:
    ShortcutFinder(Graph& g, const ContractionParameters& p);

    ShortcutContainer<Edge> FindShortcuts(Vertex& vertex);

    std::vector<Edge> FindShortcuts(Vertex& contracted_vertex, const Edge& reversed_first_edge); 

    size_t GetSearchSpaceSize();


private:
    Graph& g_;
    ContractionParameters parameters_;
    CHDijkstra<Graph> dijkstra_;

    double GetMaxOutgoingLength(Vertex& source_vertex, Vertex& contracted_vertex);

    double GetMinTargetsIngoingLength(Vertex& contracted_vertex);

    typename CHDijkstra<Graph>::TargetVerticesMap GetTargetVertices(Vertex& contracted_vertex);
};


template <typename Graph>
ShortcutFinder<Graph>::ShortcutFinder(Graph& g, const ContractionParameters& p) :
    g_(g), parameters_(p), dijkstra_(g) {}

template <typename Graph>
ShortcutContainer<typename ShortcutFinder<Graph>::Edge> ShortcutFinder<Graph>::FindShortcuts(Vertex& vertex) {
    std::vector<Edge> shortcuts{};
    ShortcutFilter filter{g_};
    for(auto&& reverse_edge : vertex.get_reverse_edges()) {
        auto&& s = FindShortcuts(vertex, reverse_edge);
        s = filter.FilterDuplicateShortcuts(s);
        shortcuts.insert(shortcuts.end(), std::make_move_iterator(s.begin()), std::make_move_iterator(s.end()));
    }
    return filter.ClassifyShortcuts(std::move(shortcuts));
}

template <typename Graph>
std::vector<typename ShortcutFinder<Graph>::Edge> ShortcutFinder<Graph>::FindShortcuts(Vertex& contracted_vertex, const Edge& reversed_first_edge) {
    std::vector<Edge> shortcuts{};
    unsigned_id_type source_vertex_id = reversed_first_edge.get_to();
    Vertex& source_vertex = g_.GetVertex(source_vertex_id);

    if (source_vertex.IsContracted()) {
        return shortcuts;
    }
    double outgoing_max_length = GetMaxOutgoingLength(source_vertex, contracted_vertex);
    if (outgoing_max_length < 0) {
        return shortcuts;
    }
    double ingoing_targets_min_length = GetMinTargetsIngoingLength(contracted_vertex);
    double max_cost = reversed_first_edge.get_length() + outgoing_max_length - ingoing_targets_min_length;
    
    typename CHDijkstra<Graph>::SearchRangeLimits limits{max_cost, parameters_.get_hop_count() - 1};
    typename CHDijkstra<Graph>::TargetVerticesMap target_vertices = GetTargetVertices(contracted_vertex);
    dijkstra_.Run(source_vertex_id, contracted_vertex.get_osm_id(), limits, target_vertices);

    for(auto&& second_edge : contracted_vertex.get_edges()) {
        unsigned_id_type target_vertex_id = second_edge.get_to();
        if (g_.GetVertex(target_vertex_id).IsContracted()) {
            continue;
        }
        double shortcut_length = reversed_first_edge.get_length() + second_edge.get_length();
        double path_length = dijkstra_.OneHopBackwardSearch(target_vertex_id);
        
        if (shortcut_length < path_length) {
            shortcuts.push_back(Edge{parameters_.NextFreeEdgeId(), source_vertex_id, target_vertex_id, shortcut_length, contracted_vertex.get_osm_id(), reversed_first_edge.get_geography()});
        }
    }
    return shortcuts;
}

template <typename Graph>
size_t ShortcutFinder<Graph>::GetSearchSpaceSize() {
    return dijkstra_.GetSearchSpaceSize();
}

template <typename Graph>
double ShortcutFinder<Graph>::GetMaxOutgoingLength(Vertex& source_vertex, Vertex& contracted_vertex) {
    double max_length = -1;
    for(auto&& edge : contracted_vertex.get_edges()) {
        bool outgoing_vertex_not_contracted = !(g_.GetVertex(edge.get_to()).IsContracted());
        bool not_edge_to_source_vertex = edge.get_to() != source_vertex.get_osm_id();
        bool bigger_length = edge.get_length() > max_length;
        if (outgoing_vertex_not_contracted && bigger_length && not_edge_to_source_vertex) {
            max_length = edge.get_length();
        }
    }
    return max_length;
}

template <typename Graph>
double ShortcutFinder<Graph>::GetMinTargetsIngoingLength(Vertex& contracted_vertex) {
    double min_length = std::numeric_limits<double>::max();
    for(auto&& edge : contracted_vertex.get_edges()) {
        auto&& target_vertex = g_.GetVertex(edge.get_to());
        if (!target_vertex.IsContracted()) {
            for(auto&& redge : target_vertex.get_reverse_edges()) {
                auto&& v = g_.GetVertex(edge.get_to());
                if (!v.IsContracted() && redge.get_length() < min_length) {
                    min_length = redge.get_length();
                }
            }
        }
        
    }
    return min_length;
}
    
template <typename Graph>
typename CHDijkstra<Graph>::TargetVerticesMap ShortcutFinder<Graph>::GetTargetVertices(Vertex& contracted_vertex) {
    typename CHDijkstra<Graph>::TargetVerticesMap target_vertices;
    for(auto&& second_edge : contracted_vertex.get_edges()) {
        unsigned_id_type target_vertex_id = second_edge.get_to();
        auto&& target_vertex = g_.GetVertex(target_vertex_id);
        if (!target_vertex.IsContracted()) {
            target_vertices.emplace(target_vertex_id, false);
        }
    }
    return target_vertices;
}




}
}

#endif //BACKEND_SHORTCUT_FINDER_H