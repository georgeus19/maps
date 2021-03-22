#include "routing/edges/ch_search_edge.h"

namespace routing {

CHSearchEdge::CHSearchEdge(): BasicEdge(), shortcut_(false), contracted_vertex_(0) {}

CHSearchEdge::CHSearchEdge(database::EdgeDbRow & r) :
    BasicEdge(r) {}

CHSearchEdge::CHSearchEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length) :
        BasicEdge(uid, from, to, length), shortcut_(false), contracted_vertex_(0) {}

CHSearchEdge::CHSearchEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length, unsigned_id_type contracted_vertex) :
        BasicEdge(uid, from, to, length), shortcut_(true), contracted_vertex_(contracted_vertex) {}


}