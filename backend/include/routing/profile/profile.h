#ifndef ROUTING_PROFILE_PROFILE_H
#define ROUTING_PROFILE_PROFILE_H

#include "routing/edges/basic_edge.h"
#include "routing/profile/preference_index.h"
#include "routing/profile/green_index.h"
#include "routing/profile/physical_length_index.h"
#include "routing/exception.h"
#include "routing/types.h"

#include "routing/database/database_helper.h"

#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <limits>

namespace routing{
namespace profile{

class Profile {
    struct Property;
public:

    Profile();

    Profile(const std::shared_ptr<PreferenceIndex>& base_index);

    void AddIndex(const std::shared_ptr<PreferenceIndex>& index, float importance);

    template <typename Graph>
    void Set(Graph& graph);

    std::string GetName() const;

    float GetLength(unsigned_id_type uid) const;

    const std::shared_ptr<PreferenceIndex>& GetBaseIndex();

    const std::shared_ptr<PreferenceIndex>& GetIndex(const std::string& name);

private:

    std::shared_ptr<PreferenceIndex> base_index_;

    std::vector<Property> properties_;

    struct Property{
        std::shared_ptr<PreferenceIndex> index;
        float importance;

        Property(const std::shared_ptr<PreferenceIndex>& ix, float im) : index(ix), importance(im) {}
    };

};

template <typename Graph>
void Profile::Set(Graph& graph) {
    graph.ForEachEdge([&](typename Graph::Edge& edge) {
        edge.set_length(GetLength(edge.get_uid()));
    });
}





}
}
#endif //ROUTING_PROFILE_PROFILE_H