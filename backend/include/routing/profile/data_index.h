#ifndef BACKEND_ROUTING_PROFILE_DATA_INDEX_H
#define BACKEND_ROUTING_PROFILE_DATA_INDEX_H

#include "routing/edges/basic_edge.h"
#include "database/database_helper.h"

#include <string>
#include <vector>
#include <cassert>
#include <limits>

namespace routing {
namespace profile {

class DataIndex{
public:
    virtual void Load(database::DatabaseHelper& d, const std::string& index_table) = 0;

    virtual void Create(database::DatabaseHelper& d, const std::vector<std::pair<unsigned_id_type, double>>& index_values, const std::string& index_table) const = 0;

    virtual void Normalize(double max) = 0;

    virtual double Get(unsigned_id_type uid) const = 0;

    virtual const std::string& GetName() const = 0;
};


}
}

#endif //BACKEND_ROUTING_PROFILE_DATA_INDEX_H