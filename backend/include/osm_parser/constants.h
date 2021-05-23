#ifndef OSM_PARSER_CONSTANTS_H
#define OSM_PARSER_CONSTANTS_H

#include <string>

namespace osm_parser {

struct Constants{

    struct Tags{
        static inline const char* kHighway = "highway";
        static inline const char* kFootway = "footway";
        static inline const char* kOneway = "oneway";
    };



    struct HighwayValues{
        static inline const std::string kPrimary = "primary";
        static inline const std::string kSecondary = "secondary";
        static inline const std::string kTertiary = "tertiary";
        static inline const std::string kUnclassified = "unclassified";
        static inline const std::string kResidential = "residential";
        static inline const std::string kPrimaryLink = "primary_link";
        static inline const std::string kSecondaryLink = "secondary_link";
        static inline const std::string kTertiaryLink = "tertiary_link";
        static inline const std::string kLivingStreet = "living_street";
        static inline const std::string kService = "service";
        static inline const std::string kPedestrian = "pedestrian";
        static inline const std::string kTrack = "track";
        static inline const std::string kFootway = "footway";
        static inline const std::string kBridleway = "bridleway";
        static inline const std::string kPath = "path";
        static inline const std::string kCycleway = "cycleway";
    };

    struct FootwayValues{
        static inline const std::string kSidewalk = "sidewalk";
        static inline const std::string kCrossing = "crossing";
    };

    struct OnewayValues{
        static inline const std::string kYes = "yes";
        static inline const std::string kTrue = "true";
        static inline const std::string k1 = "1";
    };



};



}

#endif// OSM_PARSER_CONSTANTS_H