#ifndef BACKEND_EDGE_H
#define BACKEND_EDGE_H
#include <string>
#include "database/db_edge_iterator.h"

#include <iostream>

namespace routing {
using unsigned_id_type = std::uint64_t;

/**
 * BasicEdge is a graph edge with only most general properties which can be
 * used only in simple algorithms.
 */
class BasicEdge {
public:
    enum class EdgeType { forward, backward, twoway };
   
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

    inline void set_length(double l) {
        length_ = l;
    }

    inline void SetForward() {
        type_ = EdgeType::forward;
    }

    inline void SetBackward() {
        type_ = EdgeType::backward;
    }

    inline void SetTwoway() {
        type_ = EdgeType::twoway;
    }

    BasicEdge();
    BasicEdge(database::DbEdgeIterator*);
    BasicEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length);
    BasicEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length, EdgeType type);
    BasicEdge(const BasicEdge & other) = default;
    BasicEdge(BasicEdge && other) = default;
    BasicEdge& operator= (const BasicEdge & other) = default;
    BasicEdge& operator= (BasicEdge && other) = default;
    virtual ~BasicEdge() = default;

    void Swap(BasicEdge& other);
    void Reverse(); 

    void Print() const;

    bool IsForward() const;

    bool IsBackward() const;

    bool IsTwoway() const;

    /**
     * "Graphical" comparison of edges.
     */
    bool operator==(const BasicEdge & other) const;
    bool operator!=(const BasicEdge & other) const;

protected:
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

    EdgeType type_;

private:
    std::string GetType() const;
};



}
#endif //BACKEND_EDGE_H
