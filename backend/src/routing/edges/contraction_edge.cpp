#include "routing/edges/contraction_edge.h"

namespace routing {

ContractionEdge::ContractionEdge(): BasicEdge(), shortcut_(false), contracted_vertex_(0) {}

ContractionEdge::ContractionEdge(database::EdgeDbRow & r) :
    BasicEdge(r) {}

ContractionEdge::ContractionEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length) :
        BasicEdge(uid, from, to, length), shortcut_(false), contracted_vertex_(0) {}

ContractionEdge::ContractionEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length, unsigned_id_type contracted_vertex) :
        BasicEdge(uid, from, to, length), shortcut_(true), contracted_vertex_(contracted_vertex) {}

bool ContractionEdge::operator==(const ContractionEdge & other) const {
    // if (uid_ == other.uid_) { return true; }
        // If there can be two edges with same from, to.
        if (from_ == other.from_ && to_ == other.to_ && length_ == other.length_) { return true; }
        // If a way is not oneway then there are at least 2 edges with same geography and closest edge
        // can be any of the two - there is no way to check that we selecting it.
        if (from_ == other.to_ && to_ == other.from_ && length_ == other.length_) { return true; }

        return false;
}

bool ContractionEdge::operator!=(const ContractionEdge & other) const {
    return !((*this) == other);
}


}