#include "routing/profile/pair_index_implementation.h"

#include "database/database_helper.h"
#include "routing/edges/basic_edge.h"

#include <utility>
#include <vector>
#include <string>

namespace routing{
namespace profile{

void PairIndexImplementation::Create(database::DatabaseHelper& d, const std::vector<std::pair<unsigned_id_type, double>>& index_values, const std::string& index_table,
        const std::string& value_col_name) const {
        std::string drop_table = "DROP TABLE IF EXISTS " + index_table + "; ";
        std::string create_table = "CREATE TABLE " + index_table + " ( "
                                    " uid BIGINT PRIMARY KEY, "
                                    " " + value_col_name + " DOUBLE PRECISION NOT NULL); ";
        std::string insert = "INSERT INTO " + index_table + " (uid, " + value_col_name + ") VALUES ";
        for(auto it = index_values.begin(); it != index_values.end(); ++it) {
            if (it != index_values.begin()) {
                insert += ", ";
            }
            insert += "('" + std::to_string(it->first) + "', " + std::to_string(it->second) + ")";
        }
        insert += "; ";
        std::string sql = drop_table + create_table + insert;
        d.RunTransactional(sql);
    }

void PairIndexImplementation::Normalize(double scale_max) {
    double max_value = std::numeric_limits<double>::min();
    for(auto&& p : values_) {
        if (p.valid) {
            if (max_value < p.value) {
                max_value = p.value;
            }
        }
    }
    for(auto&& p : values_) {
        if (p.valid) {
            p.value /= max_value;
            p.value *= scale_max;
        }
    }
}

double PairIndexImplementation::Get(unsigned_id_type uid) const {
    assert(uid < values_.size());
    Value v = values_[uid];
    if (v.valid) {
        return v.value;
    } else {
        throw InvalidValueException{"Index value of edge " + std::to_string(uid) + " not found - resp. invalid value is in its place."};
    }
}

std::function<void(const database::DbRow&)> PairIndexImplementation::CreateLoadFunction() {
    return [&](const database::DbRow& row) {
        unsigned_id_type uid = row.get<unsigned_id_type>(0);
        double value = row.get<double>(1);
        if (uid >= values_.size()) {
            values_.resize(uid + 1);
        }
        values_[uid] = value;
    };
}

void PairIndexImplementation::ForEachValue(const std::function<void(Value&)>& f) {
    for(auto&& v : values_) {
        f(v);
    }
}





}
}