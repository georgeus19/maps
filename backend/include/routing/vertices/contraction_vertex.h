#ifndef BACKEND_CONTRACTION_VERTEX_H
#define BACKEND_CONTRACTION_VERTEX_H
#include "routing/edges/basic_edge.h"
#include "routing/vertices/old_basic_vertex.h"
#include <vector>
#include <limits>
#include <cassert>
#include <functional>

namespace routing {

template <typename Edge>
class ContractionVertex : public OldBasicVertex<Edge> {
    using OldBasicVertex<Edge>::osm_id_;
    using OldBasicVertex<Edge>::outgoing_edges_;
public:

    using OldBasicVertex<Edge>::FindEdge;
    
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

    inline bool IsContracted() const {
        return contracted_;
    }

private:

    unsigned_id_type ordering_rank_;

    bool contracted_;

    std::vector<Edge> reverse_edges_;

};

template <typename Edge>
ContractionVertex<Edge>::ContractionVertex(unsigned_id_type osm_id)
        : OldBasicVertex<Edge>(osm_id), ordering_rank_(0), contracted_(false), reverse_edges_() {}



}

#endif //BACKEND_CONTRACTION_VERTEX_H
