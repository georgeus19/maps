#ifndef ROUTING_PROFILE_PAIR_INDEX_IMPLEMENTATION_H
#define ROUTING_PROFILE_PAIR_INDEX_IMPLEMENTATION_H

#include "routing/database/database_helper.h"
#include "routing/edges/basic_edge.h"
#include "routing/types.h"
#include "routing/exception.h"

#include <utility>
#include <vector>
#include <string>

namespace routing{
namespace profile{

/**
 * Preferences usually contain a float for each edge so only a pair of values is stored for
 * each edge - edge id and preference value.
 * 
 * Therefore, this class provides basic implementation of such pair oriented preference indices.
 */
class PairIndexImplementation{
public:
    struct Value {
        bool valid;
        float value;

        Value() : valid(false), value(0) {}
        Value(float val) : valid(true), value(val) {}
    };

    PairIndexImplementation();

    /**
     * Save new index values to database table.
     */
    void Create(database::DatabaseHelper& d, const std::vector<std::pair<unsigned_id_type, float>>& index_values, const std::string& index_table,
        const std::string& value_col_name) const;

    void Normalize();

    float Get(unsigned_id_type uid) const;

    float GetOriginal(unsigned_id_type uid) const;

    std::function<void(const database::DbRow&)> CreateLoadFunction();

    void ForEachValue(const std::function<void(Value&)>& f);

private:
    std::vector<Value> values_;
    float max_;
};

    



}
}
#endif // ROUTING_PROFILE_PAIR_INDEX_IMPLEMENTATION_H