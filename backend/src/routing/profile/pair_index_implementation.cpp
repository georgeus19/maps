#include "routing/profile/pair_index_implementation.h"

#include "database/database_helper.h"
#include "routing/edges/basic_edge.h"

#include <utility>
#include <vector>
#include <string>

namespace routing{
namespace profile{

void PairIndexImplementation::Create(database::DatabaseHelper& d, const std::vector<std::pair<unsigned_id_type, double>>& index_values, const std::string& index_table,
        const std::string& value_col_name) {
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

}
}