#ifndef BACKEND_ROUTING_EDGES_MIN_EDGE_H
#define BACKEND_ROUTING_EDGES_MIN_EDGE_H
#include <string>
#include "database/db_edge_iterator.h"

#include <iostream>

namespace routing {
using unsigned_id_type = std::uint64_t;

/**
 * MinEdge is a graph edge with only most general properties which can be
 * used only in simple algorithms.
 */
class MinEdge {
public:
   
    inline unsigned_id_type get_uid() const {
        return uid_;
    }

    inline unsigned_id_type get_to() const {
        return to_;
    }

    inline double get_length() const {
        return length_;
    }

    MinEdge();
    MinEdge(database::DbEdgeIterator*);
    MinEdge(unsigned_id_type uid, unsigned_id_type to, double length);
    MinEdge(const MinEdge & other) = default;
    MinEdge(MinEdge && other) = default;
    MinEdge& operator= (const MinEdge & other) = default;
    MinEdge& operator= (MinEdge && other) = default;
    virtual ~MinEdge() = default;

protected:
    /**
     * Unique id of the edge.
     */
    unsigned_id_type uid_;

    /**
     * Destination of the edge.
     */
    unsigned_id_type to_;

    /**
     * Length of the edge.
     */
    double length_;
};



}
#endif //BACKEND_ROUTING_EDGES_MIN_EDGE_H
