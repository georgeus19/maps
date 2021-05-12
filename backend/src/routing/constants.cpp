#include "routing/constants.h"

namespace routing{
const std::string Constants::Input::TableNames::kDatabase = "database";
const std::string  Constants::Input::TableNames::kAlgorithm = "algorithm";
const std::string  Constants::Input::TableNames::kParameters = "parameters";
const std::string  Constants::Input::TableNames::kProfileProperties = "profile_properties";
const std::string  Constants::Input::TableNames::kIndicies = "indicies";

const std::string Constants::Input::Preprocessing::kHopCount = "hop_count";
const std::string Constants::Input::Preprocessing::kEdgeDifference = "edge_difference";
const std::string Constants::Input::Preprocessing::kDeletedNeighbours = "deleted_neighbours";
const std::string Constants::Input::Preprocessing::kSpaceSize = "space_size";

const std::string Constants::Input::Database::kName = "name";
const std::string Constants::Input::Database::kUser = "user";
const std::string Constants::Input::Database::kPassword = "password";
const std::string Constants::Input::Database::kHost = "host";
const std::string Constants::Input::Database::kPort = "port";

const std::string Constants::Input::kName = "name";
const std::string Constants::Input::kTableName = "table_name";
const std::string Constants::Input::kImportance = "importance";
const std::string Constants::Input::kBaseGraphTable = "base_graph_table";

const std::string Constants::Input::Indicies::kEdgesTable = "edges_table";
const std::string Constants::Input::Indicies::kPolygonTable = "polygon_table";
const std::string Constants::Input::Indicies::kIndexTable = "index_table";

const std::string Constants::AlgorithmNames::kContractionHierarchies = "ch"; 

const std::string Constants::IndexNames::kGreenIndex = "green";
const std::string Constants::IndexNames::kLengthIndex = "length";

}