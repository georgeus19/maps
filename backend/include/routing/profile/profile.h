#ifndef BACKEND_ROUTING_PROFILE_H
#define BACKEND_ROUTING_PROFILE_H

#include "routing/edges/basic_edge.h"
#include "routing/profile/data_index.h"
#include "routing/profile/green_index.h"
#include "routing/profile/physical_length_index.h"
#include "routing/exception.h"

#include "database/database_helper.h"

#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <limits>

namespace routing{
namespace profile{

class Profile{
    struct Property;
public:

    Profile(double scale_max);

    void AddIndex(const std::shared_ptr<DataIndex>& index, int32_t importance);

    template <typename Graph>
    void Set(Graph& graph);

    void Normalize();

    std::string GetName() const;

    double GetLength(unsigned_id_type uid) const;
private:
    double scale_max_;
    std::vector<Property> properties_;
    bool normalized_;

    struct Property{
        std::shared_ptr<DataIndex> index;
        int32_t importance;

        Property(const std::shared_ptr<DataIndex>& ix, int32_t im) : index(ix), importance(im) {}

        Property(const Property& other) = default;
        Property(Property&& other) = default;
        Property& operator=(const Property& other) = default;
        Property& operator=(Property&& other) = default;
        ~Property() = default;
    };

};

template <typename Graph>
void Profile::Set(Graph& graph) {
    if (!normalized_) {
        Normalize();
    }

    graph.ForEachEdge([&](typename Graph::Edge& edge) {
        edge.set_length(GetLength(edge.get_uid()));
    });
}




}
}

#endif //BACKEND_ROUTING_PROFILE_H