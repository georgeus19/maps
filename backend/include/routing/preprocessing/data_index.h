#ifndef BACKEND_PREPROCESSING_DATA_INDEX_H
#define BACKEND_PREPROCESSING_DATA_INDEX_H

#include "routing/edges/basic_edge.h"
#include "database/database_helper.h"

#include <string>
#include <vector>
#include <cassert>
#include <limits>

namespace routing {
namespace preprocessing {

class DataIndex{
public:
    virtual void Load(const std::string& index_table, size_t max_uid) = 0;

    virtual void Normalize(double max) = 0;

    virtual double Get(unsigned_id_type uid) = 0;
};


}
}

#endif //BACKEND_PREPROCESSING_DATA_INDEX_H