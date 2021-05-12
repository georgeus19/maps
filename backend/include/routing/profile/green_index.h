#ifndef BACKEND_ROUTING_PROFILE_GREEN_INDEX_H
#define BACKEND_ROUTING_PROFILE_GREEN_INDEX_H

#include "routing/edges/basic_edge.h"
#include "database/database_helper.h"
#include "routing/profile/data_index.h"

#include <string>
#include <vector>
#include <cassert>
#include <limits>

namespace routing {
namespace profile {

class GreenIndex : public DataIndex{
public:
    GreenIndex(database::DatabaseHelper& d);

    void Create(const std::string& edges_table, const std::string& osm_polygons_table, const std::string& green_index_table);

    void Load(const std::string& green_index_table, size_t max_uid) override;

    void Normalize(double scale_max) override;

    double Get(unsigned_id_type uid) override;

    std::string GetName() const override;
private:
    struct GreenValue;
    database::DatabaseHelper& d_;
    std::vector<GreenValue> edge_green_values_;

    struct GreenValue {
        bool valid;
        double value;

        GreenValue() : valid(false), value(0) {}
        GreenValue(double val) : valid(true), value(val) {}
    };
};






}
}

#endif //BACKEND_ROUTING_PROFILE_GREEN_INDEX_H