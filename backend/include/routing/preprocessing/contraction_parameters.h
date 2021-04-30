#ifndef BACKEND_CONTRACTION_PARAMETERS_H
#define BACKEND_CONTRACTION_PARAMETERS_H

#include "routing/edges/basic_edge.h"
#include <vector>
#include <set>
#include <queue>
#include <cassert>

namespace routing {
namespace preprocessing {

class ContractionParameters {
    unsigned_id_type free_edge_id_;
    size_t hop_count_;
    int32_t edge_difference_coefficient_;
    int32_t deleted_neighbours_coefficient_;
    int32_t space_size_coefficient_;
    

public:
    ContractionParameters(unsigned_id_type free_edge_id, size_t hc, int32_t edc, int32_t dnc, int32_t ssc);

    unsigned_id_type NextFreeEdgeId();

    size_t get_hop_count() const;

    int32_t get_edge_difference_coefficient() const;

    int32_t get_deleted_neighbours_coefficient() const;

    int32_t get_space_size_coefficient() const;
};

inline ContractionParameters::ContractionParameters(unsigned_id_type free_edge_id, size_t hc, int32_t edc, int32_t dnc, int32_t ssc) :
    free_edge_id_(free_edge_id), hop_count_(hc), edge_difference_coefficient_(edc),
    deleted_neighbours_coefficient_(dnc), space_size_coefficient_(ssc) {}

inline unsigned_id_type ContractionParameters::NextFreeEdgeId() {
    ++free_edge_id_;
    return ++free_edge_id_;
}

inline size_t ContractionParameters::get_hop_count() const {
    return hop_count_;
}      

inline int32_t ContractionParameters::get_edge_difference_coefficient() const {
    return edge_difference_coefficient_;
}

inline int32_t ContractionParameters::get_deleted_neighbours_coefficient() const {
    return deleted_neighbours_coefficient_;
}

inline int32_t ContractionParameters::get_space_size_coefficient() const {
    return space_size_coefficient_;
}


}
}

#endif //BACKEND_CONTRACTION_PARAMETERS_H
