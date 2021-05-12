#ifndef ROUTING_CONSTANT_H
#define ROUTING_CONSTANT_H

#include <string>

namespace routing {

struct Constants{
    struct Input{
        struct TableNames{
            static const std::string kDatabase;
            static const std::string kAlgorithm;
            static const std::string kParameters;
            static const std::string kProfileProperties;
            static const std::string kIndicies;
        };

        struct Preprocessing {
            static const std::string kHopCount;
            static const std::string kEdgeDifference;
            static const std::string kDeletedNeighbours;
            static const std::string kSpaceSize;
        };

        struct Database{
            static const std::string kName;
            static const std::string kUser;
            static const std::string kPassword;
            static const std::string kHost;
            static const std::string kPort;
        };

        static const std::string kName;
        static const std::string kTableName;
        static const std::string kImportance;
        static const std::string kBaseGraphTable;

        struct Indicies{
            static const std::string kEdgesTable;
            static const std::string kPolygonTable;
            static const std::string kIndexTable;
        };

    };

    struct AlgorithmNames{
        static const std::string kContractionHierarchies;
    };

    struct IndexNames{
        static const std::string kGreenIndex;
        static const std::string kLengthIndex;
    };
};



}

#endif// ROUTING_CONSTANT_H