#include "routing/profile/physical_length_index.h"
#include "routing/exception.h"
#include "routing/constants.h"

#include <string>
#include <vector>
#include <cassert>
#include <limits>

namespace routing {
namespace profile {

PhysicalLengthIndex::PhysicalLengthIndex() : impl_() {}

void PhysicalLengthIndex::Create(database::DatabaseHelper& d, const std::string& edges_table, const std::string& length_index_table) {
	std::string length_index_table_index = length_index_table + "_idx";
    
    std::string sql = 
            "DROP TABLE IF EXISTS " + length_index_table + "; "
            "CREATE TABLE " + length_index_table + " AS "
            "SELECT uid, " + kValueColumnName + " "
            "FROM " + edges_table + " ; "
            "CREATE UNIQUE INDEX " + length_index_table_index  + " ON " + length_index_table + " (uid); "; 
    d.RunTransactional(sql);
}

void PhysicalLengthIndex::Load(database::DatabaseHelper& d, const std::string& index_table) {
    // edge_length_values_.assign(max_uid + 1, LengthValue{});
    std::string sql = 
            "SELECT uid, " + kValueColumnName + " "
            "FROM " + index_table + "; ";
    auto&& load = impl_.CreateLoadFunction();
    d.RunNontransactional(sql, load);
    Normalize();
}

void PhysicalLengthIndex::Create(database::DatabaseHelper& d, const std::vector<std::pair<unsigned_id_type, double>>& index_values,
        const std::string& index_table) const {
    impl_.Create(d, index_values, index_table, kValueColumnName);
}

double PhysicalLengthIndex::Get(unsigned_id_type uid) const {
    return impl_.Get(uid);
}

const std::string& PhysicalLengthIndex::GetName() const {
    return Constants::IndexNames::kLengthIndex;
}

void PhysicalLengthIndex::Normalize() {
    impl_.Normalize();
}




}
}