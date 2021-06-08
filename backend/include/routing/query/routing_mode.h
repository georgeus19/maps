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

template<typename Setup>
class RoutingMode{
public:
    virtual Router<Setup>& GetRouter(profile::Profile&& profile) = 0;
};

template<typename Setup>
class StaticProfileMode : public RoutingMode<Setup>{
public:
    StaticProfileMode()
        : routers_(), profile_() {
    }

    void AddRouter(database::DatabaseHelper& d, std::unique_ptr<TableNames>&& table_names, const profile::Profile& profile) {
        profile_ = profile;
        auto&& g = Setup::CreateGraph(d, table_names->GetEdgesTable());
        std::cout << " Loading " << table_names->GetEdgesTable() << std::endl;
        routers_.emplace(profile.GetName(), Router<Setup>{Setup{}, std::move(g), std::move(table_names)});

    }

    profile::Profile& GetDefaultProfile() {
        return profile_;
    }

    Router<Setup>& GetRouter(profile::Profile&& profile) override {
        auto it = routers_.find(profile.GetName());
        if (it != routers_.end()) {
            return it->second;
        } else {
            throw InvalidArgumentException("No route for profile " + profile.GetName() + " exists.");
        }
    }
private:
    std::unordered_map<std::string, Router<Setup>> routers_;
    profile::Profile profile_;
};


template<typename Setup>
class DynamicProfileMode : public RoutingMode<Setup>{
public:
    DynamicProfileMode(database::DatabaseHelper& d, std::unique_ptr<TableNames>&& table_names, profile::Profile&& profile)
        : router_(), profile_envelope_(std::move(profile)) {
        typename Setup::Graph g = Setup::CreateGraph(d, table_names->GetBaseTableName(), &profile_envelope_);
        std::cout << "Loading " << table_names->GetEdgesTable() << std::endl;
        router_ = std::move(Router<Setup>{Setup{}, std::move(g), std::move(table_names)});
    }

    profile::Profile& GetDefaultProfile() {
        return profile_envelope_.get_profile();
    }

    Router<Setup>& GetRouter(profile::Profile&& profile) override {
        profile_envelope_.SwitchProfile(std::move(profile));
        return router_;
    }
private:
    Router<Setup> router_;
    ProfileEnvelope profile_envelope_;


};




}
}
#endif // ROUTING_QUERY_ROUTING_MODE_H