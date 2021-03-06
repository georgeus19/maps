#include "graph_builder/edge.h"
namespace graph_builder {
    Edge::Edge(unsigned_id_type osm_id, uint64_t uid, std::string &&geography, unsigned_id_type from, unsigned_id_type to, bool undirected, const std::string& highway) :
            osm_id_(osm_id), uid_(uid), geography_(std::move(geography)), from_(from), to_(to), undirected_(undirected), highway_(highway) {}

    Edge::Edge(unsigned_id_type osm_id, uint64_t uid, const std::string & geography, unsigned_id_type from, unsigned_id_type to, bool undirected, const std::string& highway) :
        osm_id_(osm_id), uid_(uid), geography_(geography), from_(from), to_(to), undirected_(undirected), highway_(highway) {}

    /**
     * Geography is usually read later when this option of conctructor is used.
     */
    Edge::Edge(unsigned_id_type osm_id, uint64_t uid, unsigned_id_type from, unsigned_id_type to, bool undirected, const std::string& highway) :
            osm_id_(osm_id), uid_(uid), geography_(""), from_(from), to_(to), undirected_(undirected), highway_(highway) {}

    std::string Edge::get_osm_id() const {
        return std::to_string(osm_id_);
    }

    std::string Edge::get_uid() const {
        return std::to_string(uid_);
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

    std::string Edge::get_undirected() const {
        return std::to_string(undirected_);
    }

    const std::string& Edge::get_highway() const {
        return highway_;
    }
}