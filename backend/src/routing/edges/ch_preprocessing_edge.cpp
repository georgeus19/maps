#include "routing/edges/ch_preprocessing_edge.h"

namespace routing {

CHPreprocessingEdge::CHPreprocessingEdge(): BasicEdge(), shortcut_(false), contracted_vertex_(0) {}

CHPreprocessingEdge::CHPreprocessingEdge(database::EdgeDbRow & r) :
    BasicEdge(r) {}

CHPreprocessingEdge::CHPreprocessingEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length) :
        BasicEdge(uid, from, to, length), shortcut_(false), contracted_vertex_(0), geography_() {}

CHPreprocessingEdge::CHPreprocessingEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to,
    double length, unsigned_id_type contracted_vertex, const std::string& geography) :
        BasicEdge(uid, from, to, length), shortcut_(true), contracted_vertex_(contracted_vertex), geography_(geography) {}



}