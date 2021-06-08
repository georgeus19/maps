#ifndef ROUTING_CONSTANT_H
#define ROUTING_CONSTANT_H

#include <string>

namespace routing {

struct Constants{
    struct Input{
        struct TableNames{
            static inline const std::string kDatabase = "database";
            static inline const std::string kAlgorithm = "algorithm";
            static inline const std::string kParameters = "parameters";
            static inline const std::string kProfileProperties = "profile_properties";
            static inline const std::string kIndicies = "indicies";
        };

        struct Preprocessing {
            static inline const std::string kHopCount = "hop_count";
            static inline const std::string kEdgeDifference = "edge_difference";
            static inline const std::string kDeletedNeighbours = "deleted_neighbours";
            static inline const std::string kSpaceSize = "space_size";
        };

        struct Database{
            static inline const std::string kName = "name";
            static inline const std::string kUser = "user";
            static inline const std::string kPassword = "password";
            static inline const std::string kHost = "host";
            static inline const std::string kPort = "port";
        };

        static inline const std::string kName = "name";
        static inline const std::string kTableName = "table_name";
        static inline const std::string kImportance = "importance";
        static inline const std::string kBaseGraphTable = "base_graph_table";
        static inline const std::string kMode = "mode";

        struct Indicies{
            static inline const std::string kEdgesTable = "edges_table";
            static inline const std::string kPolygonTable = "polygon_table";
            static inline const std::string kPointTable = "point_table";
            static inline const std::string kIndexTable = "index_table";
        };
    };

    struct AlgorithmNames{
        static inline const std::string kContractionHierarchies = "ch";
        static inline const std::string kDijkstra = "dijkstra";
    };

    struct IndexNames{
        static inline const std::string kGreenIndex = "green";
        static inline const std::string kLengthIndex = "length";
        static inline const std::string kPeakDistanceIndex = "peak";
    };

    struct ModeNames{
        static inline const std::string kDynamicProfile = "dynamic_profile";
        static inline const std::string kStaticProfile = "static_profile";
    };
};



}

#endif// ROUTING_CONSTANT_H