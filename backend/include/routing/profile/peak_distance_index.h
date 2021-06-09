#ifndef ROUTING_PROFILE_PEAK_INDEX_H
#define ROUTING_PROFILE_PEAK_INDEX_H

#include "routing/edges/basic_edge.h"
#include "database/database_helper.h"
#include "routing/profile/data_index.h"
#include "routing/profile/pair_index_implementation.h"

#include <string>
#include <vector>
#include <cassert>
#include <limits>

namespace routing {
namespace profile {

class PeakDistanceIndex : public DataIndex{
public:
    PeakDistanceIndex();

    void Create(database::DatabaseHelper& d, const std::string& edges_table, const std::string& osm_point_table, const std::string& peak_index_table);

    void Load(database::DatabaseHelper& d, const std::string& peak_index_table) override;

    void Create(database::DatabaseHelper& d, const std::vector<std::pair<unsigned_id_type, double>>& index_values, const std::string& index_table) const override;

    double Get(unsigned_id_type uid) const override;

    const std::string& GetName() const override;
private:
    PairIndexImplementation impl_;

    static inline const std::string kValueColumnName = "peak_distance";

    static inline const double kDistanceRadius = 500;

    void Normalize() override;
};







}
}
#endif //ROUTING_PROFILE_PEAK_INDEX_H