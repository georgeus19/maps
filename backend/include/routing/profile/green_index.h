#ifndef ROUTING_PROFILE_GREEN_INDEX_H
#define ROUTING_PROFILE_GREEN_INDEX_H

#include "routing/edges/basic_edge.h"
#include "routing/database/database_helper.h"
#include "routing/profile/preference_index.h"
#include "routing/profile/pair_index_implementation.h"

#include <string>
#include <vector>
#include <cassert>
#include <limits>

namespace routing {
namespace profile {

class GreenIndex : public PreferenceIndex{
public:
    GreenIndex();

    void Create(database::DatabaseHelper& d, const std::string& edges_table, const std::string& osm_polygons_table, const std::string& green_index_table);

    void Load(database::DatabaseHelper& d, const std::string& green_index_table) override;

    void Create(database::DatabaseHelper& d, const std::vector<std::pair<unsigned_id_type, double>>& index_values, const std::string& index_table) const override;

    double Get(unsigned_id_type uid) const override;

    double GetOriginal(unsigned_id_type uid) const override;

    const std::string& GetName() const override;
private:
    PairIndexImplementation impl_;

    static inline const std::string kValueColumnName = "green_value";

    void Normalize() override;
};







}
}
#endif //ROUTING_PROFILE_GREEN_INDEX_H