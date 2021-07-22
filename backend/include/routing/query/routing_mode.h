#ifndef ROUTING_QUERY_ROUTING_MODE_H
#define ROUTING_QUERY_ROUTING_MODE_H

#include "routing/table_names.h"
#include "routing/exception.h"
#include "routing/types.h"

#include "routing/edges/length_source.h"

#include "routing/query/router.h"

#include "routing/profile/profile.h"

#include "routing/database/database_helper.h"

namespace routing{
namespace query{

/**
 * StaticProfileMode provides routing based on preferences.
 * It stores multiple Router classes that provide routing based on a profile.
 * Router classes can be added with a graph whose edge lengths are defined by a profile.
 * When routing based on a profile should be done, corresponding Router class can be retrieved.
 * 
 * @tparam AlgorithmStaticFactory Defines which routing algorithm is used in Router classes.
 */
template<typename AlgorithmStaticFactory>
class StaticProfileMode{
public:
    StaticProfileMode()
        : routers_(), profile_() {
    }

    /**
     * Add Router class which contains a graph. The graph is loaded from database table table_names.
     * The edge lengths in the table must be the same as the ones that argument profile provides so
     * that each Router class is matched to a correct profile.
     */
    void AddRouter(database::DatabaseHelper& d, std::unique_ptr<TableNames>&& table_names, const profile::Profile& profile) {
        profile_ = profile;
        auto&& g = AlgorithmStaticFactory::CreateGraph(d, table_names.get());
        std::cout << " Loading " << table_names->GetEdgesTable() << std::endl;
        routers_.emplace(profile.GetName(), Router<AlgorithmStaticFactory>{AlgorithmStaticFactory{}, std::move(g), std::move(table_names)});
    }

    profile::Profile& GetDefaultProfile() {
        return profile_;
    }

    /**
     * Retrieve previously registered Router class corresponding to the given profile.
     */
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

/**
 * DynamicProfileMode provides routig based on preferences.
 * It contains one Router class which contains a graph whose edge lengths are defined
 * by a profile stored in DynamicProfileMode. Routing for a profile can be done by
 * retrieving stored Router instance and swapping the profile with the stored to be the base
 * for the edge lengths of the graph in the Router instance.
 * 
 * @tparam AlgorithmDynamicFactory Defines which routing algorithm is used in the stored Router instance.
 */
template<typename AlgorithmDynamicFactory>
class DynamicProfileMode{
public:
    /**
     * Creates a graph from table_names table whose lengths are provided by a dynamic source - in this case the argument profile. 
     * Router instance is then created containing the graph and stored.
     */
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
    /**
     * Contains a profile which defines edge lengths for routing in Router router_.
     */
    ProfileEnvelope profile_envelope_;
};




}
}
#endif // ROUTING_QUERY_ROUTING_MODE_H