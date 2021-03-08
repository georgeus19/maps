#ifndef BACKEND_CONTRACTION_VERTEX_H
#define BACKEND_CONTRACTION_VERTEX_H
#include "routing/edges/basic_edge.h"
#include "routing/vertices/basic_vertex.h"
#include <vector>
#include <limits>
#include <cassert>
namespace routing {

template <typename Edge>
class ContractionVertex : public BasicVertex<Edge> {
private:

    unsigned_id_type order_id_;

    bool contracted_;

    std::vector<Edge> reverse_edges_;
public:
    inline std::vector<Edge>& get_reverse_edges() {
        return reverse_edges_;
    }        

    ContractionVertex(unsigned_id_type osm_id);

    inline void SetContracted() {
        assert(contracted_ == false);
        contracted_ = true;
    }

    inline bool IsContracted() {
        return contracted_;
    }

    void AddReverseEdge(Edge&& edge);

    void AddReverseEdge(const Edge & edge);

    void ForEachReverseEdge(std::function<void(Edge&)> f);

private:
    size_t GetNotContractedEdges(const std::vector<Edge>& edges);
};

template <typename Edge>
ContractionVertex<Edge>::ContractionVertex(unsigned_id_type osm_id)
        : BasicVertex<Edge>(osm_id), contracted_(false), reverse_edges_(std::vector<Edge>{}) {}

template <typename Edge>
inline void ContractionVertex<Edge>::AddReverseEdge(Edge&& edge) {
    reverse_edges_.push_back(std::move(edge));
}

template <typename Edge>
inline void ContractionVertex<Edge>::AddReverseEdge(const Edge & edge) {
    reverse_edges_.push_back(edge);
}

template <typename Edge>
void ContractionVertex<Edge>::ForEachReverseEdge(std::function<void(Edge&)> f) {
    std::for_each(reverse_edges_.begin(), reverse_edges_.end(), f);
}

// template <typename Edge>
// size_t ContractionVertex<Edge>::GetNotContractedEdges(const std::vector<Edge>& edges) {
//     size_t count = 0;
//     for(auto&& edge : edges) {
//         if (edge.)
//     }
// }


}

#endif //BACKEND_CONTRACTION_VERTEX_H
