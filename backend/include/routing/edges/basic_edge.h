#ifndef ROUTING_EDGES_BASIC_EDGE_H
#define ROUTING_EDGES_BASIC_EDGE_H
#include <string>
#include "routing/database/db_edge_iterator.h"
#include "routing/types.h"

#include <iostream>

namespace routing {


/**
 * BasicEdge is a graph edge with only most general properties which can be
 * used only in simple algorithms.
 */
template <typename LS>
class BasicEdge {
public:
    using LengthSource = LS;
    enum class EdgeType { forward, backward, twoway };
   
    inline unsigned_id_type get_uid() const {
        return uid_;
    }

    inline void set_uid(unsigned_id_type uid) {
        uid_ = uid;
    }

    inline unsigned_id_type get_from() const {
        return from_;
    }

    inline unsigned_id_type get_to() const {
        return to_;
    }

    inline float get_length() const {
        return length_.GetLength(uid_);
    }

    inline void set_length(float l) {
        length_ = LS{l};
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
    BasicEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, LengthSource length);
    BasicEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, LengthSource length, EdgeType type);
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
    LS length_;

    EdgeType type_;

private:
    std::string GetType() const;
};

template <typename LS>
BasicEdge<LS>::BasicEdge(): uid_(0), from_(0), to_(0), length_(0) {}

template <typename LS>
BasicEdge<LS>::BasicEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, LS length) :
        uid_(uid), from_(from), to_(to), length_(length), type_(EdgeType::forward) {}

template <typename LS>
BasicEdge<LS>::BasicEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, LS length, EdgeType type) :
        uid_(uid), from_(from), to_(to), length_(length), type_(type) {}

template <typename LS>
void BasicEdge<LS>::Swap(BasicEdge & other) {
    std::swap(uid_, other.uid_);
    std::swap(from_, other.from_);
    std::swap(to_, other.to_);
    std::swap(length_, other.length_);
    std::swap(type_, other.type_);
}

template <typename LS>
void BasicEdge<LS>::Reverse() {
    std::swap(from_, to_);
}

template <typename LS>
void BasicEdge<LS>::Print() const {
    std::cout << get_from() << "->" << get_to() << " length: " << get_length() << " type: " << GetType() << std::endl;
}

template <typename LS>
bool BasicEdge<LS>::IsForward() const {
    return type_ == EdgeType::forward;
}

template <typename LS>
bool BasicEdge<LS>::IsBackward() const {
    return type_ == EdgeType::backward;
}

template <typename LS>
bool BasicEdge<LS>::IsTwoway() const {
    return type_ == EdgeType::twoway;
}

template <typename LS>
bool BasicEdge<LS>::operator==(const BasicEdge& other) const {
    // if (uid_ == other.uid_) { return true; }
        // If there can be two edges with same from, to.
        if (from_ == other.from_ && to_ == other.to_ && length_.GetLength(uid_) == other.length_.GetLength(other.uid_) && type_ == other.type_) { return true; }
        // If a way is not oneway then there are at least 2 edges with same geography and closest edge
        // can be any of the two - there is no way to check that we selecting it.
        if (from_ == other.to_ && to_ == other.from_ && length_.GetLength(uid_) == other.length_.GetLength(other.uid_) && type_ == other.type_) { return true; }

        return false;
}

template <typename LS>
bool BasicEdge<LS>::operator!=(const BasicEdge& other) const {
    return !((*this) == other);
}

template <typename LS>
std::string BasicEdge<LS>::GetType() const {
    std::string s;
    switch(type_) {
        case EdgeType::forward:
            s = "forward";
            break;
        case EdgeType::backward:
            s = "backward";
            break;
        case EdgeType::twoway:
            s = "twoway";
            break;
        default:
            s = "invalid";
    }
    return s;
}




}
#endif //ROUTING_EDGES_BASIC_EDGE_H