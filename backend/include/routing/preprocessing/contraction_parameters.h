#ifndef ROUTING_PREPROCESSING_CONTRACTION_PARAMETERS_H
#define ROUTING_PREPROCESSING_CONTRACTION_PARAMETERS_H

#include "routing/edges/basic_edge.h"
#include "routing/types.h"

#include <vector>
#include <set>
#include <queue>
#include <cassert>

namespace routing {
namespace preprocessing {

class ContractionParameters {
public:
    ContractionParameters(size_t hc, int32_t edc, int32_t dnc, int32_t ssc);

    size_t get_hop_count() const;

    int32_t get_edge_difference_coefficient() const;

    int32_t get_deleted_neighbours_coefficient() const;

    int32_t get_space_size_coefficient() const;

private:
    size_t hop_count_;
    int32_t edge_difference_coefficient_;
    int32_t deleted_neighbours_coefficient_;
    int32_t space_size_coefficient_;
};

inline ContractionParameters::ContractionParameters(size_t hc, int32_t edc, int32_t dnc, int32_t ssc) :
    hop_count_(hc), edge_difference_coefficient_(edc),
    deleted_neighbours_coefficient_(dnc), space_size_coefficient_(ssc) {}

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

#endif //ROUTING_PREPROCESSING_CONTRACTION_PARAMETERS_H
