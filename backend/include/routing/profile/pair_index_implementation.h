#ifndef ROUTING_PROFILE_PAIR_INDEX_IMPLEMENTATION_H
#define ROUTING_PROFILE_PAIR_INDEX_IMPLEMENTATION_H

#include "database/database_helper.h"
#include "routing/edges/basic_edge.h"

#include <utility>
#include <vector>
#include <string>

namespace routing{
namespace profile{

class PairIndexImplementation{
public:

    void Create(database::DatabaseHelper& d, const std::vector<std::pair<unsigned_id_type, double>>& index_values, const std::string& index_table,
        const std::string& value_col_name);
};

    



}
}
#endif // ROUTING_PROFILE_PAIR_INDEX_IMPLEMENTATION_H