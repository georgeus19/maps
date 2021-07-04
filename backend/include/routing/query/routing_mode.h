#ifndef ROUTING_QUERY_ROUTING_MODE_H
#define ROUTING_QUERY_ROUTING_MODE_H

#include "routing/table_names.h"
#include "routing/exception.h"

#include "routing/edges/length_source.h"

#include "routing/query/router.h"

#include "routing/profile/profile.h"

#include "database/database_helper.h"

namespace routing{
namespace query{


template<typename AlgorithmStaticFactory>
class StaticProfileMode{
public:
    StaticProfileMode()
        : routers_(), profile_() {
    }

    void AddRouter(database::DatabaseHelper& d, std::unique_ptr<TableNames>&& table_names, const profile::Profile& profile) {
        profile_ = profile;
        auto&& g = AlgorithmStaticFactory::CreateGraph(d, table_names.get());
        std::cout << " Loading " << table_names->GetEdgesTable() << std::endl;
        routers_.emplace(profile.GetName(), Router<AlgorithmStaticFactory>{AlgorithmStaticFactory{}, std::move(g), std::move(table_names)});

    }

    profile::Profile& GetDefaultProfile() {
        return profile_;
    }

    Router<AlgorithmStaticFactory>& GetRouter(profile::Profile&& profile) {
        auto it = routers_.find(profile.GetName());
        if (it != routers_.end()) {
            return it->second;
        } else {
            throw InvalidArgumentException("No route for profile " + profile.GetName() + " exists.");
        }
    }
private:
    std::unordered_map<std::string, Router<AlgorithmStaticFactory>> routers_;
    profile::Profile profile_;
};


template<typename AlgorithmDynamicFactory>
class DynamicProfileMode{
public:
    DynamicProfileMode(database::DatabaseHelper& d, std::unique_ptr<TableNames>&& table_names, profile::Profile&& profile)
        : router_(), profile_envelope_(std::move(profile)) {
        typename AlgorithmDynamicFactory::Graph g = AlgorithmDynamicFactory::CreateGraph(d, table_names.get(), &profile_envelope_);
        std::cout << "Loading " << table_names->GetEdgesTable() << std::endl;
        router_ = std::move(Router<AlgorithmDynamicFactory>{AlgorithmDynamicFactory{}, std::move(g), std::move(table_names)});
    }

    profile::Profile& GetDefaultProfile() {
        return profile_envelope_.get_profile();
    }

    Router<AlgorithmDynamicFactory>& GetRouter(profile::Profile&& profile) {
        profile_envelope_.SwitchProfile(std::move(profile));
        return router_;
    }
private:
    Router<AlgorithmDynamicFactory> router_;
    ProfileEnvelope profile_envelope_;


};




}
}
#endif // ROUTING_QUERY_ROUTING_MODE_H