#include "routing/profile/pair_index_implementation.h"

#include "routing/database/database_helper.h"
#include "routing/edges/basic_edge.h"
#include "routing/types.h"

#include <utility>
#include <vector>
#include <string>
#include <fstream>

namespace routing{
namespace profile{

PairIndexImplementation::PairIndexImplementation() : max_(1) {}

void PairIndexImplementation::Create(database::DatabaseHelper& d, const std::vector<std::pair<unsigned_id_type, float>>& index_values, const std::string& index_table,
        const std::string& value_col_name) const {
        
    std::string drop_table = "DROP TABLE IF EXISTS " + index_table + "; ";
    std::string create_table = "CREATE TABLE " + index_table + " ( "
                                " uid INTEGER PRIMARY KEY, "
                                " " + value_col_name + " REAL NOT NULL); ";
    
    auto&& current_dir = std::filesystem::current_path();
    std::string data_path{current_dir.string() + "/" + index_table + ".csv"};
    std::string copy = "COPY " + index_table + " FROM '" + data_path + "' DELIMITER ';' CSV NULL 'null'; ";
    std::ofstream f{};

    try {
        f.open(data_path);
        for(auto&& p : index_values) {
            f << p.first << ";" << p.second << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << e.what();
        f.close();
        throw;
    }
    f.close();

    std::string sql = drop_table + create_table + copy;
    d.RunTransactional(sql);
}

void PairIndexImplementation::Normalize() {
    float max_value = std::numeric_limits<float>::min();
    for(auto&& p : values_) {
        if (p.valid) {
            if (max_value < p.value) {
                max_value = p.value;
            }
        }
    }
    max_ = max_value;
    for(auto&& p : values_) {
        if (p.valid) {
            p.value /= max_value;
        }
    }
}

float PairIndexImplementation::Get(unsigned_id_type uid) const {
    assert(uid < values_.size());
    Value v = values_[uid];
    if (v.valid) {
        return v.value;
    } else {
        throw InvalidValueException{"Index value of edge " + std::to_string(uid) + " not found - resp. invalid value is in its place."};
    }
}

float PairIndexImplementation::GetOriginal(unsigned_id_type uid) const {
    return Get(uid) * max_;
}

std::function<void(const database::DbRow&)> PairIndexImplementation::CreateLoadFunction() {
    return [&](const database::DbRow& row) {
        unsigned_id_type uid = row.get<unsigned_id_type>(0);
        float value = row.get<float>(1);
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