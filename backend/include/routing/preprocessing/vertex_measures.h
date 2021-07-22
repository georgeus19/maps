#ifndef ROUTING_PREPROCESSING_VERTEX_MEASURES_H
#define ROUTING_PREPROCESSING_VERTEX_MEASURES_H

#include "routing/edges/basic_edge.h"
#include "routing/preprocessing/ch_dijkstra.h"
#include "routing/preprocessing/contraction_parameters.h"
#include "tsl/robin_set.h"
#include "routing/preprocessing/shortcut_finder.h"
#include "routing/types.h"

#include <vector>
#include <set>
#include <queue>
#include <cassert>

namespace routing {
namespace preprocessing {

/**
 * VertexMeasures calculates any measures necessary in preprocessing of CH.
 */
template <typename Graph>
class VertexMeasures {
    using Vertex = typename Graph::Vertex;
    using Edge = typename Graph::Edge;
public:

    VertexMeasures(Graph & g, const ContractionParameters& parameters) : g_(g), shortcut_finder_(g, parameters), parameters_(parameters) {}

    int32_t CalculateEdgeDifference(Vertex& vertex); 

    int32_t CalculateEdgeDifference(Vertex& vertex, std::vector<Edge>& shortcuts);

    int32_t CalculateDeletedNeighbours(Vertex& vertex);

    float CalculateContractionAttractivity(Vertex& vertex);

    float CalculateContractionAttractivity(Vertex& vertex, std::vector<Edge>& shortcuts);

private:

    Graph& g_;
    ShortcutFinder<Graph> shortcut_finder_;
    ContractionParameters parameters_;

    void AddDeletedNeighbour(std::vector<unsigned_id_type>& deleted_neighbours, unsigned_id_type neighbour_id);

    void IncrementCurrentEdgeCount(size_t& adjacent_edges_count, unsigned_id_type neighbour_id);

    size_t CalculateCurrentEdgeCount(const std::vector<Edge>& edges);

  
};

template <typename Graph>
int32_t VertexMeasures<Graph>::CalculateEdgeDifference(Vertex& vertex) {
    auto&& shortcuts = shortcut_finder_.FindShortcuts(vertex);
    return CalculateEdgeDifference(vertex, shortcuts);
}

template <typename Graph>
int32_t VertexMeasures<Graph>::CalculateEdgeDifference(Vertex& vertex, std::vector<Edge>& shortcuts) {
    size_t adjacent_edges_count = 0;
    vertex.ForEachEdge([&](Edge& edge) {
        unsigned_id_type neighbour_id = edge.get_to();
        IncrementCurrentEdgeCount(adjacent_edges_count, neighbour_id);
    });
    vertex.ForEachBackwardEdge([&](Edge& backward_edge) {
        unsigned_id_type neighbour_id = backward_edge.get_to();
        IncrementCurrentEdgeCount(adjacent_edges_count, neighbour_id);
    });
    return shortcuts.size() - adjacent_edges_count;
}

template <typename Graph>
int32_t VertexMeasures<Graph>::CalculateDeletedNeighbours(Vertex& vertex) {
    std::vector<unsigned_id_type> deleted_neighbours{};
    vertex.ForEachEdge([&](Edge& edge) {
        unsigned_id_type neighbour_id = edge.get_to();
        AddDeletedNeighbour(deleted_neighbours, neighbour_id);
    });
    vertex.ForEachBackwardEdge([&](Edge& backward_edge) {
        unsigned_id_type neighbour_id = backward_edge.get_to();
        AddDeletedNeighbour(deleted_neighbours, neighbour_id);
    });
    return deleted_neighbours.size();
}

template <typename Graph>
inline float VertexMeasures<Graph>::CalculateContractionAttractivity(Vertex& vertex) {
    auto&& shortcuts = shortcut_finder_.FindShortcuts(vertex);
    return CalculateContractionAttractivity(vertex, shortcuts);
}

template <typename Graph>
inline float VertexMeasures<Graph>::CalculateContractionAttractivity(Vertex& vertex, std::vector<Edge>& shortcuts) {
    int32_t edge_difference = CalculateEdgeDifference(vertex, shortcuts) * parameters_.get_edge_difference_coefficient();
    int32_t deleted_neighbours = CalculateDeletedNeighbours(vertex) * parameters_.get_deleted_neighbours_coefficient();
    int32_t settled_vertices = static_cast<int32_t>(shortcut_finder_.GetSearchSpaceSize()) * parameters_.get_space_size_coefficient();
    return static_cast<float>(edge_difference + deleted_neighbours + settled_vertices);
}

template <typename Graph>
void VertexMeasures<Graph>::AddDeletedNeighbour(std::vector<unsigned_id_type>& deleted_neighbours, unsigned_id_type neighbour_id) {
    Vertex& neighbour = g_.GetVertex(neighbour_id);
    if (neighbour.IsContracted() && deleted_neighbours.end() == std::find(deleted_neighbours.begin(), deleted_neighbours.end(), neighbour_id)) {
        deleted_neighbours.push_back(neighbour_id);
    }   
}

template <typename Graph>
void VertexMeasures<Graph>::IncrementCurrentEdgeCount(size_t& adjacent_edges_count, unsigned_id_type neighbour_id) {
    Vertex& neighbour = g_.GetVertex(neighbour_id);
    if (!neighbour.IsContracted()) {
        ++adjacent_edges_count;
    }   
}







}
}

#endif //ROUTING_PREPROCESSING_VERTEX_MEASURES_H