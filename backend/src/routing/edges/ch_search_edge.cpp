#include "routing/edges/ch_search_edge.h"

namespace routing {

CHSearchEdge::CHSearchEdge(): BasicEdge(), contracted_vertex_(0) {}

CHSearchEdge::CHSearchEdge(database::DbEdgeIterator* it) :
    BasicEdge(it),
	contracted_vertex_(it->GetContractedVertex()) {}

CHSearchEdge::CHSearchEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length, EdgeType type, unsigned_id_type contracted_vertex) :
	BasicEdge(uid, from, to, length, type), contracted_vertex_(contracted_vertex) {}

CHSearchEdge::CHSearchEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length, unsigned_id_type contracted_vertex) :
	BasicEdge(uid, from, to, length), contracted_vertex_(contracted_vertex) {}

CHSearchEdge::CHSearchEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length, EdgeType type) :
	BasicEdge(uid, from, to, length, type), contracted_vertex_(0) {}

CHSearchEdge::CHSearchEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length) :
	BasicEdge(uid, from, to, length), contracted_vertex_(0) {}
	
}