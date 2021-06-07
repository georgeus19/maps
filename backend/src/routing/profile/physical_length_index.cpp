#include "routing/profile/physical_length_index.h"
#include "routing/exception.h"
#include "routing/constants.h"

#include <string>
#include <vector>
#include <cassert>
#include <limits>

namespace routing {
namespace profile {

PhysicalLengthIndex::PhysicalLengthIndex() : edge_length_values_() {}

void PhysicalLengthIndex::Load(database::DatabaseHelper& d, const std::string& index_table) {
    // edge_length_values_.assign(max_uid + 1, LengthValue{});
    std::string sql = 
            "SELECT uid, length "
            "FROM " + index_table + "; ";
    auto&& load = [&](const database::DbRow& row) {
        unsigned_id_type uid = row.get<unsigned_id_type>(0);
        double length_value = row.get<double>(1);
        if (uid >= edge_length_values_.size()) {
            edge_length_values_.resize(uid + 1);
        }
        edge_length_values_[uid] = length_value;
    };
    d.RunNontransactional(sql, load);
}

void PhysicalLengthIndex::Create(database::DatabaseHelper& d, const std::vector<std::pair<unsigned_id_type, double>>& index_values,
    const std::string& index_table) const {
    PairIndexImplementation{}.Create(d, index_values, index_table, "length");
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

double PhysicalLengthIndex::Get(unsigned_id_type uid) const {
    if (uid >= edge_length_values_.size()) {
        std::cout << uid << std::endl;
        std::cout << "edge_length_values_.size()" << edge_length_values_.size() << std::endl;
    }
    assert(uid < edge_length_values_.size());
    LengthValue lv = edge_length_values_[uid];
    if (lv.valid) {
        return lv.value;
    } else {
        throw InvalidValueException{"Green value of edge " + std::to_string(uid) + " not found - resp. invalid value is in its place."};
    }
}

const std::string& PhysicalLengthIndex::GetName() const {
    return Constants::IndexNames::kLengthIndex;
}




}
}