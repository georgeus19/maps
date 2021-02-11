#include "routing/edge.h"

namespace routing {

BasicEdge::BasicEdge(): uid_(0), from_(0), to_(0), length_(0) {}

BasicEdge::BasicEdge(database::EdgeDbRow & r) :
    uid_(r.get<unsigned_id_type>(0)),
    from_(r.get<unsigned_id_type>(1)),
    to_(r.get<unsigned_id_type>(2)),
    length_(r.get<double>(3)) {}

BasicEdge::BasicEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length) :
        uid_(uid), from_(from), to_(to), length_(length) {}

BasicEdge::BasicEdge(const BasicEdge & other) {
    uid_ = other.uid_;
    from_ = other.from_;
    to_ = other.to_;
    length_ = other.length_;
}

BasicEdge::BasicEdge(BasicEdge && other) {
    uid_ = other.uid_;
    from_ = other.from_;
    to_ = other.to_;
    length_ = other.length_;
}

BasicEdge& BasicEdge::operator= (const BasicEdge & other){
    BasicEdge tmp{other};
    Swap(tmp);
    return *this;
}

BasicEdge& BasicEdge::operator= (BasicEdge && other) {
    if (this != &other){
        uid_ = other.uid_;
        from_ = other.from_;
        to_ = other.to_;
        length_ = other.length_;
    }
    return *this;
}

BasicEdge::~BasicEdge() {}

void BasicEdge::Swap(BasicEdge & other) {
    std::swap(uid_, other.uid_);
    std::swap(from_, other.from_);
    std::swap(to_, other.to_);
    std::swap(length_, other.length_);
}

bool BasicEdge::operator==(const BasicEdge & other) const {
    if (uid_ == other.uid_) { return true; }
        // If there can be two edges with same from, to.
        if (from_ == other.from_ && to_ == other.to_ && length_ == other.length_) { return true; }
        // If a way is not oneway then there are at least 2 edges with same geography and closest edge
        // can be any of the two - there is no way to check that we selecting it.
        if (from_ == other.to_ && to_ == other.from_ && length_ == other.length_) { return true; }

        return false;
}

bool BasicEdge::operator!=(const BasicEdge & other) const {
    return !((*this) == other);
}


}