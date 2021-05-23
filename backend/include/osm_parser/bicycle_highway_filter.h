#ifndef OSM_PARSER_BICYCLE_HIGHWAY_FILTER_H
#define OSM_PARSER_BICYCLE_HIGHWAY_FILTER_H

#include <osmium/osm/way.hpp>

#include "osm_parser/highway_filter.h"
#include "osm_parser/constants.h"

#include <unordered_set>

namespace osm_parser{

class BycicleHighwayFilter : public HighwayFilter{
public: 

    BycicleHighwayFilter();

    bool FilterHighway(const osmium::TagList& tags) override;

};

BycicleHighwayFilter::BycicleHighwayFilter() {}

bool BycicleHighwayFilter::FilterHighway(const osmium::TagList& tags) {
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
            Constants::HighwayValues::kCycleway
        };

        if (bicycle_eligible_highway_values.find(highway) == bicycle_eligible_highway_values.end()) {
            return true;
        }

        const char* footway = tags.get_value_by_key(Constants::Tags::kFootway);
        if (footway && highway == Constants::HighwayValues::kFootway && std::string{footway} == Constants::FootwayValues::kSidewalk) {
            return true;
        }

        return false;
    }


}
#endif// OSM_PARSER_BICYCLE_HIGHWAY_FILTER_H