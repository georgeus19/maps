#ifndef GRAPH_BUILDER_BICYCLE_HIGHWAY_FILTER_H
#define GRAPH_BUILDER_BICYCLE_HIGHWAY_FILTER_H

#include <osmium/osm/way.hpp>

#include "graph_builder/highway_filter.h"
#include "graph_builder/constants.h"

#include <unordered_set>

namespace graph_builder {

class BycicleHighwayFilter : public HighwayFilter{
public: 

    BycicleHighwayFilter();

    bool FilterHighway(const osmium::TagList& tags) override;

};

BycicleHighwayFilter::BycicleHighwayFilter() {}

bool BycicleHighwayFilter::FilterHighway(const osmium::TagList& tags) {
    bool filter_highway = false;
    std::string highway{tags.get_value_by_key(Constants::Tags::kHighway)};
    std::unordered_set<std::string> bicycle_eligible_highway_values{
        Constants::HighwayValues::kPrimary,
        Constants::HighwayValues::kSecondary,
        Constants::HighwayValues::kTertiary,
        Constants::HighwayValues::kUnclassified,
        Constants::HighwayValues::kResidential,
        Constants::HighwayValues::kPrimaryLink,
        Constants::HighwayValues::kSecondaryLink,
        Constants::HighwayValues::kTertiaryLink,
        Constants::HighwayValues::kLivingStreet,
        Constants::HighwayValues::kService,
        Constants::HighwayValues::kPedestrian,
        Constants::HighwayValues::kTrack,
        Constants::HighwayValues::kFootway,
        Constants::HighwayValues::kBridleway,
        Constants::HighwayValues::kPath,
        Constants::HighwayValues::kCycleway,
        Constants::HighwayValues::kMiniRoundabout,
        Constants::HighwayValues::kCrossing,
        Constants::HighwayValues::kTurningLoop,
        Constants::HighwayValues::kTurningCircle
    };

    if (bicycle_eligible_highway_values.find(highway) == bicycle_eligible_highway_values.end()) {
        filter_highway = true;
    }

    return filter_highway;
}


}
#endif// GRAPH_BUILDER_BICYCLE_HIGHWAY_FILTER_H