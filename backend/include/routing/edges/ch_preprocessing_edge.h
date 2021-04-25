#ifndef BACKEND_CH_PREPROCESSING_EDGE_H
#define BACKEND_CH_PREPROCESSING_EDGE_H
#include <string>
#include "database/db_edge_iterator.h"
#include "routing/edges/basic_edge.h"


#include <iostream>

namespace routing {


class CHPreprocessingEdge : public BasicEdge {
public:
    inline bool IsShortcut() const {
        return shortcut_;
    }

    inline unsigned_id_type get_contracted_vertex() const {
        return contracted_vertex_;
    }

    CHPreprocessingEdge();
    CHPreprocessingEdge(database::DbEdgeIterator*);
    CHPreprocessingEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length, unsigned_id_type contracted_vertex);
    CHPreprocessingEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length);
    CHPreprocessingEdge(const CHPreprocessingEdge & other) = default;
    CHPreprocessingEdge(CHPreprocessingEdge && other) = default;
    CHPreprocessingEdge& operator= (const CHPreprocessingEdge & other) = default;
    CHPreprocessingEdge& operator= (CHPreprocessingEdge && other) = default;
    ~CHPreprocessingEdge() = default;

    void Swap(CHPreprocessingEdge& other);

private:
    bool shortcut_;
    unsigned_id_type contracted_vertex_;
};



}
#endif //BACKEND_CH_PREPROCESSING_EDGE_H
