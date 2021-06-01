#ifndef ROUTING_CH_SEARCH_EDGE_H
#define ROUTING_CH_SEARCH_EDGE_H
#include <string>
#include "database/db_edge_iterator.h"
#include "routing/edges/basic_edge.h"
#include <iostream>

namespace routing {

template <typename LS>
class CHEdge : public BasicEdge<LS> {
public:
    using typename BasicEdge<LS>::EdgeType;
    inline bool IsShortcut() const {
        return contracted_vertex_ != 0;
    }

    inline unsigned_id_type get_contracted_vertex() const {
        return contracted_vertex_;
    }

    CHEdge();
    CHEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, LS length, EdgeType type, unsigned_id_type contracted_vertex);
    CHEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, LS length, unsigned_id_type contracted_vertex);
    CHEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, LS length, EdgeType type);
    CHEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, LS length);
    CHEdge(const CHEdge& other) = default;
    CHEdge(CHEdge&& other) = default;
    CHEdge& operator= (const CHEdge& other) = default;
    CHEdge& operator= (CHEdge&& other) = default;
    ~CHEdge() = default;

private:
    unsigned_id_type contracted_vertex_;
};

template <typename LS>
CHEdge<LS>::CHEdge(): BasicEdge<LS>(), contracted_vertex_(0) {}

template <typename LS>
CHEdge<LS>::CHEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, LS length, EdgeType type, unsigned_id_type contracted_vertex) :
	BasicEdge<LS>(uid, from, to, length, type), contracted_vertex_(contracted_vertex) {}

template <typename LS>
CHEdge<LS>::CHEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, LS length, unsigned_id_type contracted_vertex) :
	BasicEdge<LS>(uid, from, to, length), contracted_vertex_(contracted_vertex) {}

template <typename LS>
CHEdge<LS>::CHEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, LS length, EdgeType type) :
	BasicEdge<LS>(uid, from, to, length, type), contracted_vertex_(0) {}

template <typename LS>
CHEdge<LS>::CHEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, LS length) :
	BasicEdge<LS>(uid, from, to, length), contracted_vertex_(0) {}
	





}
#endif //ROUTING_CH_SEARCH_EDGE_H
