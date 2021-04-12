#ifndef BACKEND_CONTRACTION_SEARCH_VERTEX
#define BACKEND_CONTRACTION_SEARCH_VERTEX
#include "routing/edges/basic_edge.h"
#include "routing/vertices/basic_vertex.h"
#include <vector>
#include <limits>
#include <cassert>
#include <functional>


namespace routing {


template <typename Edge>
class ContractionSearchVertex : public BasicVertex<Edge> {
private:
    using BasicVertex<Edge>::osm_id_;
    using BasicVertex<Edge>::outgoing_edges_;
    using BasicVertex<Edge>::cost_;
    using BasicVertex<Edge>::previous_;
    

    unsigned_id_type ordering_rank_;

    std::vector<Edge> reverse_edges_;

    double backward_cost_;

    unsigned_id_type backward_previous_;

public:

    using BasicVertex<Edge>::FindEdge;

    inline void set_ordering_rank(unsigned_id_type ordering_rank) {
        ordering_rank_ = ordering_rank;
    }

    inline unsigned_id_type get_ordering_rank() const {
        return ordering_rank_;
    }

    inline std::vector<Edge>& get_reverse_edges() {
        return reverse_edges_;
    }        

    inline void set_forward_cost(double cost) {
        cost_ = cost;
    }

    inline double get_forward_cost() const {
        return cost_;
    } 

    inline void set_backward_cost(double cost) {
        backward_cost_ = cost;
    }

    inline double get_backward_cost() const {
        return backward_cost_;
    }

    inline void set_forward_previous(unsigned_id_type prev) {
        previous_ = prev;
    }

    inline unsigned_id_type get_forward_previous() const {
        return previous_;
    }

    inline void set_backward_previous(unsigned_id_type prev) {
        backward_previous_ = prev;
    }

    inline unsigned_id_type get_backward_previous() const {
        return backward_previous_;
    }

    ContractionSearchVertex(unsigned_id_type osm_id);

    void AddReverseEdge(Edge&& edge);

    void AddReverseEdge(const Edge & edge);

    void ForEachReverseEdge(const std::function<void(Edge&)>& f);

    

    Edge& FindReverseEdge(const std::function<bool(const Edge&)>& f);

};

template <typename Edge>
ContractionSearchVertex<Edge>::ContractionSearchVertex(unsigned_id_type osm_id)
    : BasicVertex<Edge>(osm_id), ordering_rank_(0), reverse_edges_(),
    backward_cost_(this->GetCostMaxValue()), backward_previous_(0) {}

template <typename Edge>
inline void ContractionSearchVertex<Edge>::AddReverseEdge(Edge&& edge) {
    reverse_edges_.push_back(std::move(edge));
}

template <typename Edge>
inline void ContractionSearchVertex<Edge>::AddReverseEdge(const Edge & edge) {
    reverse_edges_.push_back(edge);
}

template <typename Edge>
void ContractionSearchVertex<Edge>::ForEachReverseEdge(const std::function<void(Edge&)>& f) {
    std::for_each(reverse_edges_.begin(), reverse_edges_.end(), f);
}

template <typename Edge>
inline Edge& ContractionSearchVertex<Edge>::FindReverseEdge(const std::function<bool(const Edge&)>& f) {
    return FindEdge(reverse_edges_, f);
}




}

#endif //BACKEND_CONTRACTION_SEARCH_VERTEX
