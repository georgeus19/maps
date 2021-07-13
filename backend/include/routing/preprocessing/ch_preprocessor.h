#ifndef ROUTING_PREPROCESSING_CH_PREPROCESSOR_H
#define ROUTING_PREPROCESSING_CH_PREPROCESSOR_H

#include "routing/preprocessing/algorithm_preprocessor.h"
#include "routing/preprocessing/contraction_parameters.h"
#include "routing/preprocessing/graph_contractor.h"

#include "routing/vertices/ch_vertex.h"
#include "routing/edge_ranges/vector_edge_range.h"
#include "routing/adjacency_list_graph.h"
#include "routing/bidirectional_graph.h"
#include "routing/table_names.h"
#include "routing/edges/length_source.h"
#include "routing/edge_factory.h"
#include "routing/types.h"

#include "routing/profile/profile.h"

#include "routing/database/database_helper.h"

#include <functional>

namespace routing{
namespace preprocessing{

class CHPreprocessor{
    using Edge = CHEdge<NumberLengthSource>;
public:
    using Graph = BidirectionalGraph<AdjacencyListGraph<CHVertex<Edge, VectorEdgeRange<Edge>>, Edge>>;

    CHPreprocessor(std::reference_wrapper<database::DatabaseHelper> d, TableNames* table_names, ContractionParameters&& parameters)
        : d_(d), table_names_(std::move(table_names)), parameters_(std::move(parameters)) {}

    void RunPreprocessing(Graph& g) {
        std::cout << "Vertices: " << g.GetVertexCount() << std::endl;
        std::cout << "Edges before contraction: " << g.GetEdgeCount() << std::endl;
        GraphContractor<Graph> c{g, parameters_, g.GetMaxEdgeId() + 1};
        c.ContractGraph();
        std::cout << "Contraction done." << std::endl;
        std::cout << "Edges after contraction: " << g.GetEdgeCount() << std::endl;
    }

    Graph LoadGraph(profile::Profile& profile) {
        std::cout << "Load graph from " << table_names_->GetBaseTableName() << "." << std::endl;
        Graph g{};
        database::UnpreprocessedDbGraph unpreprocessed_db_graph{};
        CHNumberEdgeFactory edge_factory{};
        d_.get().LoadGraphEdges<Graph>(table_names_->GetBaseTableName(), g, &unpreprocessed_db_graph, edge_factory);
        std::cout << "Profile: " << profile.GetName() << std::endl;
        profile.Set(g);
        return g;
    }

    void SaveGraph(Graph& g) {
        database::CHDbGraph ch_db_graph{};
        std::string ch_edges_table{table_names_->GetEdgesTable()};
        std::string ch_vertex_table{table_names_->GetVerticesTable()};
        d_.get().CreateGraphTable(table_names_->GetBaseTableName(), ch_edges_table, &ch_db_graph);
        d_.get().DropGeographyIndex(ch_edges_table);
        std::cout << "Geography index dropped." << std::endl;
        d_.get().AddShortcuts(ch_edges_table, g);
        std::cout << "Shortcuts added to " + ch_edges_table << "."<< std::endl;
        d_.get().CreateGeographyIndex(ch_edges_table);
        std::cout << "Geography index restored." << std::endl;
        d_.get().AddVertexOrdering(ch_vertex_table, g);
        std::cout << "Vertex ordering saved to " << ch_vertex_table << "." << std::endl;
    }
private:
    std::reference_wrapper<database::DatabaseHelper> d_;
    TableNames* table_names_;
    ContractionParameters parameters_;


};





}
}
#endif // ROUTING_PREPROCESSING_CH_PREPROCESSOR_H