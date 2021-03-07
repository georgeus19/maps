#ifndef BACKEND_CONTRACTION_PARAMETERS
#define BACKEND_CONTRACTION_PARAMETERS

#include "routing/edge.h"
#include <vector>
#include <set>
#include <queue>
#include <cassert>

namespace routing {
namespace preprocessing {

class ContractionParameters {
    unsigned_id_type free_edge_id_;

public:
    unsigned_id_type NextFreeEdgeId();

    ContractionParameters(unsigned_id_type free_edge_id);

};

inline unsigned_id_type ContractionParameters::NextFreeEdgeId() {
    return ++free_edge_id_;
}

}
}

#endif //BACKEND_CONTRACTION_PARAMETERS
