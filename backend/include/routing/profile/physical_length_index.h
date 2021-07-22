#ifndef ROUTING_PROFILE_PHYSICAL_LENGTH_INDEX_H
#define ROUTING_PROFILE_PHYSICAL_LENGTH_INDEX_H

#include "routing/edges/basic_edge.h"
#include "routing/profile/preference_index.h"
#include "routing/profile/pair_index_implementation.h"
#include "routing/types.h"

#include "routing/database/database_helper.h"

#include <string>
#include <vector>
#include <cassert>
#include <limits>

namespace routing {
namespace profile {

/**
 * PhysicalLengthIndex represents real distances (lengths) of edges. 
 * 
 * Note that there are edges with distances of hunderds of meters
 * and edges with distances of a few meters. If this index is normalized and
 * added to other prefences via addition operation, the routing does not work
 * very well due to the big relative range of values. 
 * 
 * This index works much better if it serves as a base index and values of 
 * other prefence indicies are instead multiplied with it.
 */
class PhysicalLengthIndex : public PreferenceIndex{
public:
    PhysicalLengthIndex();

    void Create(database::DatabaseHelper& d, const std::string& edges_table, const std::string& length_index_table);

    void Load(database::DatabaseHelper& d, const std::string& green_index_table) override;

    void Create(database::DatabaseHelper& d, const std::vector<std::pair<unsigned_id_type, float>>& index_values, const std::string& index_table) const override;

    float Get(unsigned_id_type uid) const override;

    float GetOriginal(unsigned_id_type uid) const override;

    const std::string& GetName() const override;
    
private:
    PairIndexImplementation impl_;

    static inline const std::string kValueColumnName = "length";

    void Normalize() override;
};







}
}
#endif //ROUTING_PROFILE_PHYSICAL_LENGTH_INDEX_H