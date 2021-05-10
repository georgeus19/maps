#ifndef BACKEND_ROUTING_PROFILE_PHYSICAL_LENGTH_INDEX_H
#define BACKEND_ROUTING_PROFILE_PHYSICAL_LENGTH_INDEX_H

#include "routing/edges/basic_edge.h"
#include "database/database_helper.h"
#include "routing/profile/data_index.h"

#include <string>
#include <vector>
#include <cassert>
#include <limits>

namespace routing {
namespace profile {

class PhysicalLengthIndex : public DataIndex{
public:
    PhysicalLengthIndex(database::DatabaseHelper& d);

    void Load(const std::string& green_index_table, size_t max_uid) override;

    void Normalize(double scale_max) override;

    double Get(unsigned_id_type uid) override;
private:
    struct LengthValue;
    database::DatabaseHelper& d_;
    std::vector<LengthValue> edge_length_values_;

    struct LengthValue {
        bool valid;
        double value;

        LengthValue() : valid(false), value(0) {}
        LengthValue(double val) : valid(true), value(val) {}
    };
};






}
}

#endif //BACKEND_ROUTING_PROFILE_PHYSICAL_LENGTH_INDEX_H