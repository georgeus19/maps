#ifndef BACKEND_EDGE_H
#define BACKEND_EDGE_H
#include <string>
#include "database/database_helper.h"

#include <iostream>

namespace routing {
using unsigned_id_type = std::uint64_t;

/**
 * BasicEdge is a graph edge with only most general properties which can be
 * used only in simple algorithms.
 */
class BasicEdge {
private:
    using id_type = unsigned_id_type;

    /**
     * Unique id of the edge.
     */
    unsigned_id_type uid_;

    /**
     * Origin of the edge.
     */
    unsigned_id_type from_;

    /**
     * Destination of the edge.
     */
    unsigned_id_type to_;

    /**
     * Length of the edge.
     */
    double length_;
public:
   
    inline unsigned_id_type get_uid() const {
        return uid_;
    }

    inline unsigned_id_type get_from() const {
        return from_;
    }

    inline unsigned_id_type get_to() const {
        return to_;
    }

    inline double get_length() const {
        return length_;
    }

    BasicEdge();
    BasicEdge(database::EdgeDbRow &);
    BasicEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length);
    BasicEdge(const BasicEdge & other);
    BasicEdge(BasicEdge && other);
    BasicEdge& operator= (const BasicEdge & other);
    BasicEdge& operator= (BasicEdge && other);
    ~BasicEdge();

    void Swap(BasicEdge & other);
    void Reverse(); 

    void Print();

    /**
     * "Graphical" comparison of edges.
     */
    bool operator==(const BasicEdge & other) const;
    bool operator!=(const BasicEdge & other) const;
};



}
#endif //BACKEND_EDGE_H
