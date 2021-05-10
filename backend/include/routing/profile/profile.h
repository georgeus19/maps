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
public:

    Profile(double scale_max);

    void AddIndex(DataIndex* index, double importance);

    template <typename Graph>
    void Set(Graph& graph);

    void Normalize();

private:
    struct Property;
    double scale_max_;
    std::vector<Property> properties_;
    bool normalized_;

    struct Property{
        DataIndex* index;
        double importance;

        Property(DataIndex* ix, double im) : index(ix), importance(im) {}

        Property(const Property& other) = default;
        Property(Property&& other) = default;
        Property& operator=(const Property& other) = default;
        Property& operator=(Property&& other) = default;
        ~Property() = default;
    };

    double GetLength(unsigned_id_type uid);
};

template <typename Graph>
void Profile::Set(Graph& graph) {
    if (!normalized_) {
        Normalize();
    }

    graph.ForEachEdge([](typename Graph::Edge& edge) {
        edge.set_length(GetLength(edge.get_uid()));
    });
}




}
}

#endif //BACKEND_ROUTING_PROFILE_H