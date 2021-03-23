#ifndef BACKEND_CH_SEARCH_EDGE_H
#define BACKEND_CH_SEARCH_EDGE_H
#include <string>
#include "database/database_helper.h"
#include "routing/edges/basic_edge.h"
#include <iostream>

namespace routing {


class CHSearchEdge : public BasicEdge {
public:
    inline bool IsShortcut() const {
        return shortcut_;
    }

    inline unsigned_id_type get_contracted_vertex() const {
        return contracted_vertex_;
    }

    CHSearchEdge();
    CHSearchEdge(database::EdgeDbRow &);
    CHSearchEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length, unsigned_id_type contracted_vertex);
    CHSearchEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length);
    CHSearchEdge(const CHSearchEdge & other) = default;
    CHSearchEdge(CHSearchEdge && other) = default;
    CHSearchEdge& operator= (const CHSearchEdge & other) = default;
    CHSearchEdge& operator= (CHSearchEdge && other) = default;
    ~CHSearchEdge() = default;

private:
    bool shortcut_;
    unsigned_id_type contracted_vertex_;
};



}
#endif //BACKEND_CH_SEARCH_EDGE_H