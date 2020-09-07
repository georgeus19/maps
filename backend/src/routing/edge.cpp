#include "routing/edge.h"

namespace routing {
    BasicEdge::BasicEdge(unsigned_id_type osm_id, std::string &&geography, unsigned_id_type from, unsigned_id_type to, double length) :
            osm_id_(osm_id), geography_(geography), from_(std::move(from)), to_(to), length_(length) {}

    BasicEdge::BasicEdge(const BasicEdge & other) {
        osm_id_ = other.osm_id_;
        geography_ = other.geography_;
        from_ = other.from_;
        to_ = other.to_;
        length_ = other.length_;
    }

    BasicEdge::BasicEdge(BasicEdge && other) {
    osm_id_ = other.osm_id_;
    geography_ = std::move(other.geography_);
    other.geography_ = std::string{};
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
            osm_id_ = other.osm_id_;
            geography_ = std::move(other.geography_);
            other.geography_ = std::string{};
            from_ = other.from_;
            to_ = other.to_;
            length_ = other.length_;
        }
        return *this;
    }

    BasicEdge::~BasicEdge() {}

    void BasicEdge::Swap(BasicEdge & other) {
        std::swap(osm_id_, other.osm_id_);
        std::swap(geography_, other.geography_);
        std::swap(from_, other.from_);
        std::swap(to_, other.to_);
        std::swap(length_, other.length_);
    }
}
