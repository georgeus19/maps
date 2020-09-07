#ifndef BACKEND_EDGE_H
#define BACKEND_EDGE_H
#include <string>

#include <iostream>

namespace routing {
    using unsigned_id_type = std::uint64_t;

    class BasicEdge {
    public:
        unsigned_id_type osm_id_;
        std::string geography_;
        unsigned_id_type from_;
        unsigned_id_type to_;
        double length_;

        BasicEdge(unsigned_id_type osm_id, std::string &&geography, unsigned_id_type from, unsigned_id_type to, double length);

        BasicEdge(unsigned_id_type osm_id, unsigned_id_type from, unsigned_id_type to);

        BasicEdge(const BasicEdge & other);

        BasicEdge(BasicEdge && other);

        BasicEdge& operator= (const BasicEdge & other);

        BasicEdge& operator= (BasicEdge && other);

        ~BasicEdge();

        void Swap(BasicEdge & other);

        unsigned_id_type get_osm_id() const;

        const std::string &get_geography() const;

        void set_geography(std::string &&geog);

        unsigned_id_type get_from() const;

        unsigned_id_type get_to() const;

    };

    inline unsigned_id_type BasicEdge::get_osm_id() const {
        return osm_id_;
    }

    inline const std::string &BasicEdge::get_geography() const {
        return geography_;
    }

    inline void BasicEdge::set_geography(std::string &&geog) {
        geography_ = std::move(geog);
    }

    inline unsigned_id_type BasicEdge::get_from() const {
        return from_;
    }

    inline unsigned_id_type BasicEdge::get_to() const {
        return to_;
    }

}
#endif //BACKEND_EDGE_H
