#include "routing/edges/min_edge.h"

namespace routing {

MinEdge::MinEdge(): uid_(0), to_(0), length_(0) {}

MinEdge::MinEdge(database::DbEdgeIterator* it) :
    uid_(it->GetUid()),
    to_(it->GetTo()),
    length_(it->GetLength()) {}

MinEdge::MinEdge(unsigned_id_type uid, unsigned_id_type to, double length) :
        uid_(uid), to_(to), length_(length) {}




}