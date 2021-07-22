#ifndef ROUTING_PROFILE_PREFERENCE_INDEX_H
#define ROUTING_PROFILE_PREFERENCE_INDEX_H

#include "routing/edges/basic_edge.h"
#include "routing/types.h"

#include "routing/database/database_helper.h"

#include <string>
#include <vector>
#include <cassert>
#include <limits>

namespace routing {
namespace profile {

/**
 * Interface for any preference for edges. The lower the value, the more preferred it is.
 * If the other preference is desired, use GetInverted method instead to get inverted preferences.
 */
class PreferenceIndex{
public:
    /**
     * Load index from database. This method must be called before Create, Get, GetOriginal, GetInverted are called.
     */
    virtual void Load(database::DatabaseHelper& d, const std::string& index_table) = 0;

    /**
     * Save bew index values to database table.
     */
    virtual void Create(database::DatabaseHelper& d, const std::vector<std::pair<unsigned_id_type, float>>& index_values, const std::string& index_table) const = 0;


    /**
     * Get normalized preference value of an edge.
     */
    virtual float Get(unsigned_id_type uid) const = 0;

    /**
     * Values are usually normalized so this returns the original preference value of an edge.
     */
    virtual float GetOriginal(unsigned_id_type uid) const = 0;

    virtual const std::string& GetName() const = 0;

    float GetInverted(unsigned_id_type uid) const {
        return kScaleMax - Get(uid);
    }

    virtual ~PreferenceIndex() = default;

protected:
    /**
     * Denotes the maximum value of any value of all data indices.
     */
    static inline const float kScaleMax = 1;

private:
    virtual void Normalize() = 0;

};



}
}
#endif //ROUTING_PROFILE_PREFERENCE_INDEX_H