#ifndef ROUTING_PROFILE_PREFERENCE_INDEX_H
#define ROUTING_PROFILE_PREFERENCE_INDEX_H

#include "routing/edges/basic_edge.h"
#include "database/database_helper.h"

#include <string>
#include <vector>
#include <cassert>
#include <limits>

namespace routing {
namespace profile {

class PreferenceIndex{
public:
    virtual void Load(database::DatabaseHelper& d, const std::string& index_table) = 0;

    virtual void Create(database::DatabaseHelper& d, const std::vector<std::pair<unsigned_id_type, double>>& index_values, const std::string& index_table) const = 0;

    virtual double Get(unsigned_id_type uid) const = 0;

    virtual const std::string& GetName() const = 0;

    double GetInverted(unsigned_id_type uid) const {
        return kScaleMax - Get(uid);
    }

protected:
    /**
     * Denotes the maximum value of any value of all data indices.
     */
    static inline const double kScaleMax = 100;

private:
    virtual void Normalize() = 0;

};



}
}
#endif //ROUTING_PROFILE_PREFERENCE_INDEX_H