#ifndef ROUTING_PREPROCESSING_SHORTCUT_FINDER_H
#define ROUTING_PREPROCESSING_SHORTCUT_FINDER_H

#include "routing/edges/basic_edge.h"
#include "routing/preprocessing/ch_dijkstra.h"
#include "routing/types.h"
#include "routing/preprocessing/contraction_parameters.h"
#include "tsl/robin_set.h"
#include "tsl/robin_map.h"
#include "routing/preprocessing/shortcut_filter.h"

#include <vector>
#include <iterator>
#include <set>
#include <queue>
#include <cassert>
#include <cstdlib>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>
#include <random>
#include <cmath>
#include <iostream>
#include <ctime>

namespace routing {
namespace preprocessing {

/**
 * ShortcutFinder finds shortcuts that should be added if a vertex were to be contracted.
 */
template <typename Graph>
class ShortcutFinder {
    using Vertex = typename Graph::Vertex;
    using Edge = typename Graph::Edge;
public:
    ShortcutFinder(Graph& g, const ContractionParameters& p);

    /**
     * Find shortcuts that should be added if a vertex were to be contracted.
     */
    std::vector<Edge> FindShortcuts(Vertex& vertex);

    /**
     * Find shortcuts that should be added if a vertex were to be contracted that would go though backward_former_edge edge.
     */
    std::vector<Edge> FindShortcuts(Vertex& contracted_vertex, const Edge& backward_former_edge); 

    size_t GetSearchSpaceSize();

private:
    Graph& g_;
    ContractionParameters parameters_;
    CHDijkstra<Graph> dijkstra_;
    unsigned_id_type temporary_edge_id_counter_;

    float GetMaxOutgoingLength(Vertex& source_vertex, Vertex& contracted_vertex);

    float GetMinTargetsIngoingLength(Vertex& contracted_vertex);

    typename CHDijkstra<Graph>::TargetVerticesMap GetTargetVertices(Vertex& contracted_vertex);
};


template <typename Graph>
ShortcutFinder<Graph>::ShortcutFinder(Graph& g, const ContractionParameters& p) :
    g_(g), parameters_(p), dijkstra_(g), temporary_edge_id_counter_(1) {}

template <typename Graph>
std::vector<typename ShortcutFinder<Graph>::Edge> ShortcutFinder<Graph>::FindShortcuts(Vertex& vertex) {
    std::vector<Edge> shortcuts{};
    ShortcutFilter<Graph> filter{g_};
    vertex.ForEachBackwardEdge([&](Edge& backward_edge) {
        auto&& s = FindShortcuts(vertex, backward_edge);
        s = filter.FilterDuplicateShortcuts(s);
        shortcuts.insert(shortcuts.end(), std::make_move_iterator(s.begin()), std::make_move_iterator(s.end()));
    });
    return filter.MergeUnorderedShortcuts(shortcuts);
}

template <typename Graph>
std::vector<typename ShortcutFinder<Graph>::Edge> ShortcutFinder<Graph>::FindShortcuts(Vertex& contracted_vertex, const Edge& backward_former_edge) {
    std::vector<Edge> shortcuts{};

    unsigned_id_type source_vertex_id = backward_former_edge.get_to();
    Vertex& source_vertex = g_.GetVertex(source_vertex_id);

    if (source_vertex.IsContracted()) {
        return shortcuts;
    }
    float outgoing_max_length = GetMaxOutgoingLength(source_vertex, contracted_vertex);
    if (outgoing_max_length < 0) {
        return shortcuts;
    }
    float ingoing_targets_min_length = GetMinTargetsIngoingLength(contracted_vertex);
    float max_cost = backward_former_edge.get_length() + outgoing_max_length - ingoing_targets_min_length;
    
    typename CHDijkstra<Graph>::SearchRangeLimits limits{max_cost, parameters_.get_hop_count() - 1};
    typename CHDijkstra<Graph>::TargetVerticesMap target_vertices = GetTargetVertices(contracted_vertex);
    dijkstra_.Run(source_vertex_id, contracted_vertex.get_uid(), limits, target_vertices);

    contracted_vertex.ForEachEdge([&](Edge& latter_edge) {
        unsigned_id_type target_vertex_id = latter_edge.get_to();
        if (g_.GetVertex(target_vertex_id).IsContracted()) {
            return;
        }
        float shortcut_length = backward_former_edge.get_length() + latter_edge.get_length();
        float path_length = dijkstra_.OneHopBackwardSearch(target_vertex_id);
        
        if (shortcut_length < path_length) {
            shortcuts.emplace_back(++temporary_edge_id_counter_, source_vertex_id, target_vertex_id, shortcut_length, contracted_vertex.get_uid());
        }
    });

    return shortcuts;
}

template <typename Graph>
size_t ShortcutFinder<Graph>::GetSearchSpaceSize() {
    return dijkstra_.GetSearchSpaceSize();
}

template <typename Graph>
float ShortcutFinder<Graph>::GetMaxOutgoingLength(Vertex& source_vertex, Vertex& contracted_vertex) {
    float max_length = -1;
    contracted_vertex.ForEachEdge([&](Edge& edge) {
        bool outgoing_vertex_not_contracted = !(g_.GetVertex(edge.get_to()).IsContracted());
        bool not_edge_to_source_vertex = edge.get_to() != source_vertex.get_uid();
        bool bigger_length = edge.get_length() > max_length;
        if (outgoing_vertex_not_contracted && bigger_length && not_edge_to_source_vertex) {
            max_length = edge.get_length();
        }
    });
    return max_length;
}

template <typename Graph>
float ShortcutFinder<Graph>::GetMinTargetsIngoingLength(Vertex& contracted_vertex) {
    float min_length = std::numeric_limits<float>::max();
    contracted_vertex.ForEachEdge([&](Edge& edge) {
        auto&& target_vertex = g_.GetVertex(edge.get_to());
        if (!target_vertex.IsContracted()) {
            target_vertex.ForEachBackwardEdge([&](Edge& backward_edge) {
                auto&& v = g_.GetVertex(backward_edge.get_to());
                if (!v.IsContracted() && backward_edge.get_length() < min_length) {
                    min_length = backward_edge.get_length();
                }
            });
        }
    });
    return min_length;
}
    
template <typename Graph>
typename CHDijkstra<Graph>::TargetVerticesMap ShortcutFinder<Graph>::GetTargetVertices(Vertex& contracted_vertex) {
    typename CHDijkstra<Graph>::TargetVerticesMap target_vertices;
    contracted_vertex.ForEachEdge([&](Edge& latter_edge) {
        unsigned_id_type target_vertex_id = latter_edge.get_to();
        auto&& target_vertex = g_.GetVertex(target_vertex_id);
        if (!target_vertex.IsContracted()) {
            target_vertices.emplace(target_vertex_id, false);
        }
    });
    return target_vertices;
}





}
}
#endif //ROUTING_PREPROCESSING_SHORTCUT_FINDER_H