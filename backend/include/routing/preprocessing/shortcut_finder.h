#ifndef BACKEND_SHORTCUT_FINDER
#define BACKEND_SHORTCUT_FINDER

#include "routing/edges/basic_edge.h"
#include "routing/preprocessing/ch_dijkstra.h"
#include <vector>
#include <set>
#include <queue>
#include <cassert>
#include "routing/preprocessing/contraction_parameters.h"
#include "tsl/robin_set.h"

namespace routing {
namespace preprocessing {

template <typename Graph>
class ShortcutFinder {
    using Vertex = Graph::V;
    using Edge = Graph::E;
public:
    ShortcutFinder(Graph& g, const ContractionParameters& p);

    std::vector<Edge> FindShortcuts(Vertex& vertex);

    std::vector<Edge> FindShortcuts(Vertex& contracted_vertex, const Edge& reversed_first_edge); 

    size_t GetSearchSpaceSize();
private:
    Graph& g_;
    ContractionParameters parameters_;
    CHDijkstra<Graph> dijkstra_;

    double GetMaxOutgoingLength(Vertex& source_vertex, Vertex& contracted_vertex);

    double GetMinTargetsIngoingLength(Vertex& contracted_vertex);

    std::vector<Edge> FilterDuplicateEdges(const std::vector<Edge>& edges);

};


template <typename Graph>
ShortcutFinder<Graph>::ShortcutFinder(Graph& g, const ContractionParameters& p) :
    g_(g), parameters_(p), dijkstra_(g) {}

template <typename Graph>
std::vector<typename ShortcutFinder<Graph>::Edge> ShortcutFinder<Graph>::FindShortcuts(Vertex& vertex) {
    std::vector<Edge> shortcuts{};
    for(auto&& reverse_edge : vertex.get_reverse_edges()) {
        auto&& s = FindShortcuts(vertex, reverse_edge);
        s = FilterDuplicateEdges(s);
        shortcuts.insert(shortcuts.end(), s.begin(), s.end());
    }
    return shortcuts;
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
    tsl::robin_set<unsigned_id_type> target_vertices;
    for(auto&& second_edge : contracted_vertex.get_edges()) {
        unsigned_id_type target_vertex_id = second_edge.get_to();
        auto&& target_vertex = g_.GetVertex(target_vertex_id);
        if (!target_vertex.IsContracted()) {
            target_vertices.insert(target_vertex_id);
        }
    }
    // CHDijkstra<Graph> dijkstra{g_};
    dijkstra_.Run(source_vertex_id, contracted_vertex.get_osm_id(), typename CHDijkstra<Graph>::SearchRangeLimits{max_cost, parameters_.get_hop_count() - 1}, target_vertices);

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
std::vector<typename ShortcutFinder<Graph>::Edge> ShortcutFinder<Graph>::FilterDuplicateEdges(const std::vector<Edge>& edges) {
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





}
}

#endif //BACKEND_SHORTCUT_FINDER