#ifndef BACKEND_CONTRACTION_EDGE_H
#define BACKEND_CONTRACTION_EDGE_H
#include <string>
#include "database/database_helper.h"
#include "routing/edges/basic_edge.h"
#include <iostream>

namespace routing {


class ContractionEdge : public BasicEdge {
public:
    inline bool IsShortcut() const {
        return shortcut_;
    }

    inline unsigned_id_type get_contracted_vertex() const {
        return contracted_vertex_;
    }

    ContractionEdge();
    ContractionEdge(database::EdgeDbRow &);
    ContractionEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length, unsigned_id_type contracted_vertex);
    ContractionEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length);
    ContractionEdge(const ContractionEdge & other) = default;
    ContractionEdge(ContractionEdge && other) = default;
    ContractionEdge& operator= (const ContractionEdge & other) = default;
    ContractionEdge& operator= (ContractionEdge && other) = default;
    ~ContractionEdge() = default;

    void Swap(ContractionEdge & other);

    /**
     * "Graphical" comparison of edges.
     */
    bool operator==(const ContractionEdge & other) const;
    bool operator!=(const ContractionEdge & other) const;

private:
    bool shortcut_;
    unsigned_id_type contracted_vertex_;
};



}
#endif //BACKEND_CONTRACTION_EDGE_H
