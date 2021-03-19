#ifndef BACKEND_CONTRACTION_VERTEX_H
#define BACKEND_CONTRACTION_VERTEX_H
#include "routing/edges/basic_edge.h"
#include "routing/vertices/basic_vertex.h"
#include <vector>
#include <limits>
#include <cassert>
#include <functional>

namespace routing {

template <typename Edge>
class ContractionVertex : public BasicVertex<Edge> {
public:
    inline void set_ordering_rank(unsigned_id_type ordering_rank) {
        ordering_rank_ = ordering_rank;
    }

    inline unsigned_id_type get_ordering_rank() const {
        return ordering_rank_;
    }

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

    void ForEachReverseEdge(const std::function<void(Edge&)>& f);
private:

    unsigned_id_type ordering_rank_;

    bool contracted_;

    std::vector<Edge> reverse_edges_;

};

template <typename Edge>
ContractionVertex<Edge>::ContractionVertex(unsigned_id_type osm_id)
        : BasicVertex<Edge>(osm_id), ordering_rank_(0), contracted_(false), reverse_edges_(std::vector<Edge>{}) {}

template <typename Edge>
inline void ContractionVertex<Edge>::AddReverseEdge(Edge&& edge) {
    reverse_edges_.push_back(std::move(edge));
}

template <typename Edge>
inline void ContractionVertex<Edge>::AddReverseEdge(const Edge & edge) {
    reverse_edges_.push_back(edge);
}

template <typename Edge>
void ContractionVertex<Edge>::ForEachReverseEdge(const std::function<void(Edge&)>& f) {
    std::for_each(reverse_edges_.begin(), reverse_edges_.end(), f);
}



}

#endif //BACKEND_CONTRACTION_VERTEX_H
