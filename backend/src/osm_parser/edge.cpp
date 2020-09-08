//
// Created by hrubyk on 02.09.20.
//

#include "osm_parser/edge.h"
namespace osm_parser {
    Edge::Edge(unsigned_id_type osm_id, std::string &&geography, unsigned_id_type from, unsigned_id_type to) :
            osm_id_(osm_id), geography_(std::move(geography)), from_(from), to_(to) {}

    Edge::Edge(unsigned_id_type osm_id, const std::string & geography, unsigned_id_type from, unsigned_id_type to) :
        osm_id_(osm_id), geography_(geography), from_(from), to_(to) {}

    Edge::Edge(unsigned_id_type osm_id, unsigned_id_type from, unsigned_id_type to) :
            osm_id_(osm_id), geography_(""), from_(from), to_(to) {}

    std::string Edge::get_osm_id() const {
        return std::to_string(osm_id_);
    }

    const std::string &Edge::get_geography() const {
        return geography_;
    }

    void Edge::set_geography(std::string &&geog) {
        geography_ = std::move(geog);
    }

    std::string Edge::get_from() const {
        return std::to_string(from_);
    }

    std::string Edge::get_to() const {
        return std::to_string(to_);
    }
}