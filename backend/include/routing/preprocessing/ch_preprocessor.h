#ifndef ROUTING_PREPROCESSING_CH_PREPROCESSOR_H
#define ROUTING_PREPROCESSING_CH_PREPROCESSOR_H

#include "routing/preprocessing/algorithm_preprocessor.h"
#include "routing/preprocessing/contraction_parameters.h"
#include "routing/preprocessing/graph_contractor.h"

#include "routing/vertices/ch_vertex.h"
#include "routing/edge_ranges/vector_edge_range.h"
#include "routing/adjacency_list_graph.h"
#include "routing/bidirectional_graph.h"
#include "routing/table_name_repository.h"
#include "routing/edges/length_source.h"
#include "routing/edge_factory.h"

#include "routing/profile/profile.h"

#include "database/database_helper.h"

#include <functional>

namespace routing{
namespace preprocessing{

class CHPreprocessor : public AlgorithmPreprocessor{
    using Edge = CHEdge<NumberLengthSource>;
    using Graph = BidirectionalGraph<AdjacencyListGraph<CHVertex<Edge, VectorEdgeRange<Edge>>, Edge>>;
public:

    CHPreprocessor(database::DatabaseHelper&& d, std::unique_ptr<TableNames>&& table_names, ContractionParameters&& parameters)
        : d_(std::move(d)), table_names_(std::move(table_names)), parameters_(std::move(parameters)) {}

    void RunPreprocessing(profile::Profile& profile) override {
        Graph g = LoadGraph(profile);
        std::cout << "Vertices: " << g.GetVertexCount() << std::endl;
        std::cout << "Edges before contraction: " << g.GetEdgeCount() << std::endl;
        GraphContractor<Graph> c{g, parameters_, g.GetMaxEdgeId() + 1};
        c.ContractGraph();
        std::cout << "Contraction done." << std::endl;
        std::cout << "Edges after contraction: " << g.GetEdgeCount() << std::endl;
        Save(g);
    }

private:
    database::DatabaseHelper d_;
    ContractionParameters parameters_;
    std::unique_ptr<TableNames> table_names_;

    Graph LoadGraph(profile::Profile& profile) {
        std::cout << "Load graph from " << table_names_->GetBaseTableName() << "." << std::endl;
        Graph g{};
        database::UnpreprocessedDbGraph unpreprocessed_db_graph{};
        CHNumberEdgeFactory edge_factory{};
        d_.LoadGraphEdges<Graph>(table_names_->GetBaseTableName(), g, &unpreprocessed_db_graph, edge_factory);
        std::cout << "Profile: " << profile.GetName() << std::endl;
        profile.Set(g);
        return g;
    }

    void Save(Graph& g) {
        database::CHDbGraph ch_db_graph{};
        std::string ch_edges_table{table_names_->GetEdgesTable()};
        std::string ch_vertex_table{table_names_->GetVerticesTable()};
        d_.CreateGraphTable(table_names_->GetBaseTableName(), ch_edges_table, &ch_db_graph);
        d_.DropGeographyIndex(ch_edges_table);
        std::cout << "Geography index dropped." << std::endl;
        d_.AddShortcuts(ch_edges_table, g);
        std::cout << "Shortcuts added to " + ch_edges_table << "."<< std::endl;
        d_.CreateGeographyIndex(ch_edges_table);
        std::cout << "Geography index restored." << std::endl;
        d_.AddVertexOrdering(ch_vertex_table, g);
        std::cout << "Vertex ordering saved to " << ch_vertex_table << "." << std::endl;
    }

};





}
}
#endif // ROUTING_PREPROCESSING_CH_PREPROCESSOR_H