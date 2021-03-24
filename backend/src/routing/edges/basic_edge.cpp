#include "routing/edges/basic_edge.h"

namespace routing {

BasicEdge::BasicEdge(): uid_(0), from_(0), to_(0), length_(0) {}

BasicEdge::BasicEdge(database::DbEdgeIterator* it) :
    uid_(it->GetUid()),
    from_(it->GetFrom()),
    to_(it->GetTo()),
    length_(it->GetLength()) {}

BasicEdge::BasicEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length) :
        uid_(uid), from_(from), to_(to), length_(length) {}

void BasicEdge::Swap(BasicEdge & other) {
    std::swap(uid_, other.uid_);
    std::swap(from_, other.from_);
    std::swap(to_, other.to_);
    std::swap(length_, other.length_);
}

void BasicEdge::Reverse() {
    std::swap(from_, to_);
}

void BasicEdge::Print() {
    std::cout << get_from() << "->" << get_to() << " length: " << get_length() << std::endl;
}

bool BasicEdge::operator==(const BasicEdge& other) const {
    // if (uid_ == other.uid_) { return true; }
        // If there can be two edges with same from, to.
        if (from_ == other.from_ && to_ == other.to_ && length_ == other.length_) { return true; }
        // If a way is not oneway then there are at least 2 edges with same geography and closest edge
        // can be any of the two - there is no way to check that we selecting it.
        if (from_ == other.to_ && to_ == other.from_ && length_ == other.length_) { return true; }

        return false;
}

bool BasicEdge::operator!=(const BasicEdge& other) const {
    return !((*this) == other);
}


}