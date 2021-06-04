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

class IProfile{
public:
    virtual double GetLength(unsigned_id_type uid) const = 0;
};

class Profile : public IProfile{
    struct Property;
public:

    Profile(double scale_max);

    void AddIndex(const std::shared_ptr<DataIndex>& index, int32_t importance);

    template <typename Graph>
    void Set(Graph& graph);

    void Normalize();

    std::string GetName() const;

    double GetLength(unsigned_id_type uid) const override;

    std::shared_ptr<DataIndex> GetIndex(const std::string& name) {
        auto it = std::find_if(properties_.begin(), properties_.end(), [&](const Property& p){
            return p.index->GetName() == name;
        });
        if (it != properties_.end()) {
            return it->index;
        } else {
            return std::shared_ptr<DataIndex>();
        }
    }

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