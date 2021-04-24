#ifndef BACKEND_CONTRACTION_SEARCH_VERTEX
#define BACKEND_CONTRACTION_SEARCH_VERTEX
#include "routing/edges/basic_edge.h"
#include "routing/vertices/old_basic_vertex.h"
#include <vector>
#include <limits>
#include <cassert>
#include <functional>


namespace routing {


template <typename Edge>
class ContractionSearchVertex : public OldBasicVertex<Edge> {
private:
    using OldBasicVertex<Edge>::osm_id_;
    using OldBasicVertex<Edge>::outgoing_edges_;
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

    ContractionSearchVertex(unsigned_id_type osm_id);
private:
    unsigned_id_type ordering_rank_;

    std::vector<Edge> reverse_edges_;
};

template <typename Edge>
ContractionSearchVertex<Edge>::ContractionSearchVertex(unsigned_id_type osm_id)
    : OldBasicVertex<Edge>(osm_id), ordering_rank_(0), reverse_edges_() {}

}

#endif //BACKEND_CONTRACTION_SEARCH_VERTEX
