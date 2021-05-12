#include "routing/profile/physical_length_index.h"
#include "routing/exception.h"

#include <string>
#include <vector>
#include <cassert>
#include <limits>

namespace routing {
namespace profile {

PhysicalLengthIndex::PhysicalLengthIndex(database::DatabaseHelper& d) : d_(d), edge_length_values_() {}

void PhysicalLengthIndex::Load(const std::string& index_table, size_t max_uid) {
    edge_length_values_.assign(max_uid + 1, LengthValue{});
    std::string sql = 
            "SELECT uid, length "
            "FROM " + index_table + "; ";
    auto&& load = [&](const database::DbRow& row) {
        unsigned_id_type uid = row.get<unsigned_id_type>(0);
        double length_value = row.get<double>(1);
        edge_length_values_[uid] = length_value;
    };
    d_.RunNontransactional(sql, load);
}

void PhysicalLengthIndex::Normalize(double scale_max) {
    double max_value = std::numeric_limits<double>::min();
    for(auto&& length_value : edge_length_values_) {
        if (length_value.valid) {
            if (max_value < length_value.value) {
                max_value = length_value.value;
            }
        }
    }
    for(auto&& length_value : edge_length_values_) {
        if (length_value.valid) {
            length_value.value /= max_value;
            length_value.value *= scale_max;
        }
    }
}

double PhysicalLengthIndex::Get(unsigned_id_type uid) {
    assert(uid < edge_length_values_.size());
    LengthValue lv = edge_length_values_[uid];
    if (lv.valid) {
        return lv.value;
    } else {
        throw InvalidValueException{"Green value of edge " + std::to_string(uid) + " not found - resp. invalid value is in its place."};
    }
}

std::string PhysicalLengthIndex::GetName() const {
    return "length";
}




}
}