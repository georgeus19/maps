#ifndef VERTEX_MEASURES_H
#define VERTEX_MEASURES_H

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
class VertexMeasures {
    using Vertex = Graph::V;
    using Edge = Graph::E;
public:

    VertexMeasures(Graph & g, const ContractionParameters& parameters) : g_(g), parameters_(parameters), dijkstra_(g) {}

    std::vector<Edge> FindShortcuts(Vertex& vertex);

    std::vector<Edge> FindShortcuts(Vertex& contracted_vertex, const Edge& reversed_first_edge); 

    int32_t CalculateEdgeDifference(Vertex& vertex); 

    int32_t CalculateEdgeDifference(Vertex& vertex, std::vector<Edge>& shortcuts);

    int32_t CalculateDeletedNeighbours(Vertex& vertex);

    double CalculateContractionAttractivity(Vertex& vertex);

    double CalculateContractionAttractivity(Vertex& vertex, std::vector<Edge>& shortcuts);

private:

    Graph & g_;
    ContractionParameters parameters_;
    CHDijkstra<Graph> dijkstra_;

    void CalculateDeletedNeighbours(const std::vector<Edge>& edges, std::vector<unsigned_id_type>& counted_vertices);

    size_t CalculateCurrentEdgeCount(const std::vector<Edge>& edges);

    double GetMaxOutgoingLength(Vertex& source_vertex, Vertex& contracted_vertex);

    double GetMinTargetsIngoingLength(Vertex& contracted_vertex);

    std::vector<Edge> FilterDuplicateEdges(const std::vector<Edge>& edges);

};

template <typename Graph>
std::vector<typename VertexMeasures<Graph>::Edge> VertexMeasures<Graph>::FindShortcuts(Vertex& vertex) {
    std::vector<Edge> shortcuts{};
    for(auto&& reverse_edge : vertex.get_reverse_edges()) {
        auto&& s = FindShortcuts(vertex, reverse_edge);
        s = FilterDuplicateEdges(s);
        shortcuts.insert(shortcuts.end(), s.begin(), s.end());
    }
    return shortcuts;
}

template <typename Graph>
std::vector<typename VertexMeasures<Graph>::Edge> VertexMeasures<Graph>::FindShortcuts(Vertex& contracted_vertex, const Edge& reversed_first_edge) {
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
int32_t VertexMeasures<Graph>::CalculateEdgeDifference(Vertex& vertex) {
    auto&& shortcuts = FindShortcuts(vertex);
    return CalculateEdgeDifference(vertex, shortcuts);
}

template <typename Graph>
int32_t VertexMeasures<Graph>::CalculateEdgeDifference(Vertex& vertex, std::vector<Edge>& shortcuts) {
    size_t adjacent_edges_count = CalculateCurrentEdgeCount(vertex.get_edges()) + CalculateCurrentEdgeCount(vertex.get_reverse_edges());
    return shortcuts.size() - adjacent_edges_count;
}

template <typename Graph>
int32_t VertexMeasures<Graph>::CalculateDeletedNeighbours(Vertex& vertex) {
    std::vector<unsigned_id_type> deleted_neighbours{};
    CalculateDeletedNeighbours(vertex.get_edges(), deleted_neighbours);
    CalculateDeletedNeighbours(vertex.get_reverse_edges(), deleted_neighbours);
    return deleted_neighbours.size();
}

template <typename Graph>
inline double VertexMeasures<Graph>::CalculateContractionAttractivity(Vertex& vertex) {
    auto&& shortcuts = FindShortcuts(vertex);
    return CalculateContractionAttractivity(vertex, shortcuts);
}

template <typename Graph>
inline double VertexMeasures<Graph>::CalculateContractionAttractivity(Vertex& vertex, std::vector<Edge>& shortcuts) {
    int32_t edge_difference = CalculateEdgeDifference(vertex, shortcuts) * parameters_.get_edge_difference_coefficient();
    int32_t deleted_neighbours = CalculateDeletedNeighbours(vertex) * parameters_.get_deleted_neighbours_coefficient();
    int32_t settled_vertices = dijkstra_.GetSearchSpaceSize() * parameters_.get_space_size_coefficient();
    return static_cast<double>(edge_difference + deleted_neighbours + settled_vertices);
}

template <typename Graph>
void VertexMeasures<Graph>::CalculateDeletedNeighbours(const std::vector<Edge>& edges, std::vector<unsigned_id_type>& counted_vertices) {
    for(auto&& edge : edges) {
        unsigned_id_type neighbour_id = edge.get_to();
        Vertex& neighbour = g_.GetVertex(neighbour_id);
        if (neighbour.IsContracted() && counted_vertices.end() == std::find(counted_vertices.begin(), counted_vertices.end(), neighbour_id)) {
            counted_vertices.push_back(neighbour_id);
        }   
    }
}

template <typename Graph>
size_t VertexMeasures<Graph>::CalculateCurrentEdgeCount(const std::vector<Edge>& edges) {
    size_t count = 0;
    for(auto&& edge : edges) {
        Vertex& vertex = g_.GetVertex(edge.get_to());
        if (!vertex.IsContracted()) {
            ++count;
        }   
    }
    return count;
}

template <typename Graph>
double VertexMeasures<Graph>::GetMaxOutgoingLength(Vertex& source_vertex, Vertex& contracted_vertex) {
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
double VertexMeasures<Graph>::GetMinTargetsIngoingLength(Vertex& contracted_vertex) {
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
std::vector<typename VertexMeasures<Graph>::Edge> VertexMeasures<Graph>::FilterDuplicateEdges(const std::vector<Edge>& edges) {
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

#endif //VERTEX_MEASURES_H