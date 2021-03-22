#ifndef BACKEND_CH_PREPROCESSING_EDGE_H
#define BACKEND_CH_PREPROCESSING_EDGE_H
#include <string>
#include "database/database_helper.h"
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

    inline const std::string& get_geography() const {
        return geography_;
    }

    CHPreprocessingEdge();
    CHPreprocessingEdge(database::EdgeDbRow &);
    CHPreprocessingEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length, unsigned_id_type contracted_vertex, const std::string& geography);
    CHPreprocessingEdge(unsigned_id_type uid, unsigned_id_type from, unsigned_id_type to, double length);
    CHPreprocessingEdge(const CHPreprocessingEdge & other) = default;
    CHPreprocessingEdge(CHPreprocessingEdge && other) = default;
    CHPreprocessingEdge& operator= (const CHPreprocessingEdge & other) = default;
    CHPreprocessingEdge& operator= (CHPreprocessingEdge && other) = default;
    ~CHPreprocessingEdge() = default;


private:
    bool shortcut_;
    unsigned_id_type contracted_vertex_;
    std::string geography_;
};



}
#endif //BACKEND_CH_PREPROCESSING_EDGE_H
