#ifndef BACKEND_VERTEX_MEASURES_H
#define BACKEND_VERTEX_MEASURES_H

#include "routing/edges/basic_edge.h"
#include "routing/preprocessing/ch_dijkstra.h"
#include <vector>
#include <set>
#include <queue>
#include <cassert>
#include "routing/preprocessing/contraction_parameters.h"
#include "tsl/robin_set.h"
#include "routing/preprocessing/shortcut_finder.h"

namespace routing {
namespace preprocessing {

template <typename Graph>
class VertexMeasures {
    using Vertex = Graph::Vertex;
    using Edge = Graph::Edge;
public:

    VertexMeasures(Graph & g, const ContractionParameters& parameters) : g_(g), shortcut_finder_(g, parameters), parameters_(parameters) {}

    int32_t CalculateEdgeDifference(Vertex& vertex); 

    int32_t CalculateEdgeDifference(Vertex& vertex, std::vector<Edge>& shortcuts);

    int32_t CalculateDeletedNeighbours(Vertex& vertex);

    double CalculateContractionAttractivity(Vertex& vertex);

    double CalculateContractionAttractivity(Vertex& vertex, std::vector<Edge>& shortcuts);

private:

    Graph& g_;
    ShortcutFinder<Graph> shortcut_finder_;
    ContractionParameters parameters_;

    void CalculateDeletedNeighbours(const std::vector<Edge>& edges, std::vector<unsigned_id_type>& counted_vertices);

    size_t CalculateCurrentEdgeCount(const std::vector<Edge>& edges);

  
};

template <typename Graph>
int32_t VertexMeasures<Graph>::CalculateEdgeDifference(Vertex& vertex) {
    auto&& shortcuts = shortcut_finder_.FindShortcuts(vertex);
    return CalculateEdgeDifference(vertex, shortcuts.new_edges);
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
    auto&& shortcuts = shortcut_finder_.FindShortcuts(vertex);
    return CalculateContractionAttractivity(vertex, shortcuts.new_edges);
}

template <typename Graph>
inline double VertexMeasures<Graph>::CalculateContractionAttractivity(Vertex& vertex, std::vector<Edge>& shortcuts) {
    int32_t edge_difference = CalculateEdgeDifference(vertex, shortcuts) * parameters_.get_edge_difference_coefficient();
    int32_t deleted_neighbours = CalculateDeletedNeighbours(vertex) * parameters_.get_deleted_neighbours_coefficient();
    int32_t settled_vertices = static_cast<int32_t>(shortcut_finder_.GetSearchSpaceSize()) * parameters_.get_space_size_coefficient();
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







}
}

#endif //BACKEND_VERTEX_MEASURES_H