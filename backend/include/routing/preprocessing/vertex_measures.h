#ifndef VERTEX_MEASURES_H
#define VERTEX_MEASURES_H

#include "routing/edges/basic_edge.h"
#include "routing/preprocessing/ch_dijkstra.h"
#include <vector>
#include <set>
#include <queue>
#include <cassert>
#include "routing/preprocessing/contraction_parameters.h"

namespace routing {
namespace preprocessing {

template <typename Graph>
class VertexMeasures {
    using Vertex = Graph::V;
    using Edge = Graph::E;
    Graph & g_;
    ContractionParameters parameters_;
    
public:

    VertexMeasures(Graph & g, const ContractionParameters& parameters) : g_(g), parameters_(parameters) {}

    std::vector<Edge> FindShortcuts(Vertex & vertex);

    void FindShortcuts(std::vector<Edge>& shortcuts, Vertex & contracted_vertex, const Edge & reversed_first_edge); 

    int32_t CalculateEdgeDifference(Vertex& vertex); 

    int32_t CalculateEdgeDifference(Vertex& vertex, std::vector<Edge>& shortcuts);

    int32_t CalculateDeletedNeighbours(Vertex& vertex);

    double CalculateContractionAttractivity(Vertex& vertex);

    double CalculateContractionAttractivity(Vertex& vertex, std::vector<Edge>& shortcuts);

private:

    void CalculateDeletedNeighbours(const std::vector<Edge>& edges, std::vector<unsigned_id_type>& counted_vertices);

    size_t CalculateCurrentEdgeCount(const std::vector<Edge>& edges);


    double GetMaxOutgoingLength(Vertex& source_vertex, Vertex& contracted_vertex);

};

template <typename Graph>
std::vector<typename VertexMeasures<Graph>::Edge> VertexMeasures<Graph>::FindShortcuts(Vertex & vertex) {
    std::vector<Edge> shortcuts{};
    for(auto&& reverse_edge : vertex.get_reverse_edges()) {
        FindShortcuts(shortcuts, vertex, reverse_edge);
    }
    return shortcuts;
}

template <typename Graph>
void VertexMeasures<Graph>::FindShortcuts(std::vector<Edge>& shortcuts, Vertex & contracted_vertex, const Edge & reversed_first_edge) {
    unsigned_id_type source_vertex_id = reversed_first_edge.get_to();
    Vertex& source_vertex = g_.GetVertex(source_vertex_id);

    if (source_vertex.IsContracted()) {
        return;
    }
    double outgoing_max_length = GetMaxOutgoingLength(source_vertex, contracted_vertex);
    if (outgoing_max_length < 0) {
        return;
    }
    double max_cost = reversed_first_edge.get_length() + outgoing_max_length;
    auto&& end_condition = [=](Vertex* v) {
        return v->get_cost() > max_cost;
    };
    CHDijkstra<Graph> dijkstra{g_};
    dijkstra.Run(source_vertex_id, contracted_vertex.get_osm_id(), typename CHDijkstra<Graph>::SearchRangeLimits{max_cost, parameters_.get_hop_count()});

    for(auto&& second_edge : contracted_vertex.get_edges()) {
        double shortcut_length = reversed_first_edge.get_length() + second_edge.get_length();
        unsigned_id_type end_vertex_id = second_edge.get_to();
        double path_length = dijkstra.GetPathLength(end_vertex_id);
        
        if (!g_.GetVertex(end_vertex_id).IsContracted() && shortcut_length < path_length) {
            shortcuts.push_back(Edge{parameters_.NextFreeEdgeId(), source_vertex_id, end_vertex_id, shortcut_length, contracted_vertex.get_osm_id(), reversed_first_edge.get_geography()});
        }
    }
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
    return static_cast<double>(edge_difference + deleted_neighbours);
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


}
}

#endif //VERTEX_MEASURES_H
