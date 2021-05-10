#ifndef BACKEND_PREPROCESSING_CH_DATA_MANAGER_H
#define BACKEND_PREPROCESSING_CH_DATA_MANAGER_H

#include "routing/exception.h"
#include "routing/adjacency_list_graph.h"
#include "routing/bidirectional_graph.h"
#include "routing/edges/basic_edge.h"
#include "routing/edges/ch_preprocessing_edge.h"
#include "routing/edge_ranges/vector_edge_range.h"
#include "routing/vertices/ch_vertex.h"

#include "routing/profile/data_index.h"

#include "database/db_graph.h"
#include "database/database_helper.h"

#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <limits>

namespace routing {
namespace preprocessing {

class CHDataManager {
public:
    using Graph = BidirectionalGraph<AdjacencyListGraph<CHVertex<CHPreprocessingEdge, VectorEdgeRange<CHPreprocessingEdge>>, CHPreprocessingEdge>>;
    
    CHDataManager(database::DatabaseHelper& d);

    Graph LoadBaseGraph(const std::string& graph_table);

    void SaveNewGraph(Graph& g, const std::string& base_graph_table, const std::string& output_graph_table);

private:
    database::DatabaseHelper& d_;
};

CHDataManager::CHDataManager(database::DatabaseHelper& d) : d_(d) {}

CHDataManager::Graph CHDataManager::LoadBaseGraph(const std::string& graph_table) {
    Graph g{};
    database::UnpreprocessedDbGraph unpreprocessed_db_graph{};
    d_.LoadFullGraph<Graph>(graph_table, g, &unpreprocessed_db_graph);
    return g;
}

void CHDataManager::SaveNewGraph(Graph& g, const std::string& base_graph_table, const std::string& output_graph_table) {
    std::string output_ordering_table{output_graph_table + "_vertex_ordering"};
    database::CHDbGraph ch_db_graph{};
    d_.CreateGraphTable(base_graph_table, output_graph_table, &ch_db_graph);

    d_.DropGeographyIndex(output_graph_table);
    std::cout << "Geography index dropped." << std::endl;
    
    d_.AddShortcuts(output_graph_table, g);
    std::cout << "Shortcuts added to " + output_graph_table << "."<< std::endl;
    d_.CreateGeographyIndex(output_graph_table);
    std::cout << "Geography index restored." << std::endl;

    d_.AddVertexOrdering(output_ordering_table, g);
    std::cout << "Vertex ordering saved to " << output_ordering_table << "." << std::endl;
}



}
}

#endif //BACKEND_PREPROCESSING_CH_DATA_MANAGER_H