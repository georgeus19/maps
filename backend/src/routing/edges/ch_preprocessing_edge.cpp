#include "routing/edges/ch_preprocessing_edge.h"

namespace routing {

CHPreprocessingEdge::CHPreprocessingEdge(): BasicEdge(), shortcut_(false), contracted_vertex_(0) {}

CHPreprocessingEdge::CHPreprocessingEdge(database::DbEdgeIterator*it) :
    BasicEdge(it),
    shortcut_(it->GetShortcut()),
    contracted_vertex_(it->GetContractedVertex()) {}

CHPreprocessingEdge::CHPreprocessingEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length, EdgeType type) :
        BasicEdge(uid, from, to, length, type), shortcut_(false), contracted_vertex_(0) {}

CHPreprocessingEdge::CHPreprocessingEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, 
    double length, EdgeType type, unsigned_id_type contracted_vertex) :
        BasicEdge(uid, from, to, length, type), shortcut_(true), contracted_vertex_(contracted_vertex) {}

CHPreprocessingEdge::CHPreprocessingEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, 
    double length, unsigned_id_type contracted_vertex) :
        BasicEdge(uid, from, to, length), shortcut_(true), contracted_vertex_(contracted_vertex) {}

CHPreprocessingEdge::CHPreprocessingEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, 
    double length) :
        BasicEdge(uid, from, to, length), shortcut_(true), contracted_vertex_(0) {}

void CHPreprocessingEdge::Swap(CHPreprocessingEdge& other) {
    BasicEdge::Swap(other);
    std::swap(shortcut_, other.shortcut_);
    std::swap(contracted_vertex_, other.contracted_vertex_);
}

}