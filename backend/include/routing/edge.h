#ifndef BACKEND_EDGE_H
#define BACKEND_EDGE_H
#include <string>
#include "database/database_helper.h"

#include <iostream>

namespace routing {
    using unsigned_id_type = std::uint64_t;

    class BasicEdge {
    public:
        using id_type = unsigned_id_type;
        unsigned_id_type uid_;
        unsigned_id_type from_;
        unsigned_id_type to_;
        double length_;

        BasicEdge();

        BasicEdge(database::EdgeDbRow &);

        BasicEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length);

        BasicEdge(const BasicEdge & other);

        BasicEdge(BasicEdge && other);

        BasicEdge& operator= (const BasicEdge & other);

        BasicEdge& operator= (BasicEdge && other);

        ~BasicEdge();

        void Swap(BasicEdge & other);

        bool operator==(const BasicEdge & other) const;

        bool operator!=(const BasicEdge & other) const;

        unsigned_id_type get_uid() const;

        unsigned_id_type get_from() const;

        unsigned_id_type get_to() const;

    };

    inline unsigned_id_type BasicEdge::get_uid() const {
        return uid_;
    }

    inline unsigned_id_type BasicEdge::get_from() const {
        return from_;
    }

    inline unsigned_id_type BasicEdge::get_to() const {
        return to_;
    }

}
#endif //BACKEND_EDGE_H
