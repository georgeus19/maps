#ifndef ROUTING_PROFILE_ROAD_TYPE_INDEX_H
#define ROUTING_PROFILE_ROAD_TYPE_INDEX_H

#include "routing/edges/basic_edge.h"
#include "database/database_helper.h"
#include "routing/profile/preference_index.h"
#include "routing/profile/pair_index_implementation.h"

#include <string>
#include <vector>
#include <cassert>
#include <limits>

namespace routing {
namespace profile {

class RoadTypeIndex : public PreferenceIndex{
public:
    RoadTypeIndex();

    void Create(database::DatabaseHelper& d, const std::string& edges_table, const std::string& index_table);

    void Load(database::DatabaseHelper& d, const std::string& peak_index_table) override;

    void Create(database::DatabaseHelper& d, const std::vector<std::pair<unsigned_id_type, double>>& index_values, const std::string& index_table) const override;

    double Get(unsigned_id_type uid) const override;

    double GetOriginal(unsigned_id_type uid) const override;

    const std::string& GetName() const override;
private:
    PairIndexImplementation impl_;

    static inline const std::string kValueColumnName = "highway";

    void Normalize() override;
};







}
}
#endif //ROUTING_PROFILE_ROAD_TYPE_INDEX_H