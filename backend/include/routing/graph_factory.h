#ifndef ROUTING_GRAPH_FACTORY_H
#define ROUTING_GRAPH_FACTORY_H

#include "database/db_graph.h"
#include "database/database_helper.h"
#include "routing/table_name_repository.h"
#include "routing/edge_factory.h"

namespace routing{

template <typename EdgeFactory, typename EdgeRange>
class BasicGraphFactory{
public:
    using Edge = typename EdgeFactory::Edge;
    using Vertex = CHVertex<Edge, EdgeRange>;
    
    using Graph = BidirectionalGraph<AdjacencyListGraph<Vertex, Edge>>;

    BasicGraphFactory(const EdgeFactory& ef, database::DatabaseHelper& d, database::DbGraph* db_graph)
        : edge_factory_(ef), d_(d), db_graph_(db_graph) {} 

    Graph CreateGraph(const TableNameRepository& table_names) {
        Graph g{};
        auto&& it = d_.get().LoadGraphEdges<Graph>(table_names.GetBaseTableName(), g, &db_graph_);
        for (; !(it->IsEnd()); it->Inc()) {
            g.AddEdge(edge_factory_.Create(*it));
        }
        return g;
    }  

private:
    EdgeFactory edge_factory_;
    std::reference_wrapper<database::DatabaseHelper> d_;
    database::DbGraph* db_graph_;
};

template <typename EdgeFactory, typename EdgeRange>
class CHGraphFactory{
public:
    using Edge = typename EdgeFactory::Edge;
    using Vertex = CHVertex<Edge, EdgeRange>;
    
    using Graph = BidirectionalGraph<AdjacencyListGraph<Vertex, Edge>>;

    CHGraphFactory(const EdgeFactory& ef, database::DatabaseHelper& d, database::DbGraph* db_graph)
        : edge_factory_(ef), d_(d), db_graph_(db_graph) {} 

    Graph CreateGraph(const TableNameRepository& table_names) {
        Graph g{};
        auto&& it = d_.get().LoadGraphEdges<Graph>(table_names.GetBaseTableName(), g, &db_graph_);
        for (; !(it->IsEnd()); it->Inc()) {
            g.AddEdge(edge_factory_.Create(*it));
        }
        d_.get().LoadAdditionalVertexProperties(table_names.GetBaseTableName() + "_vertices", g);
        return g;
    }  

private:
    EdgeFactory edge_factory_;
    std::reference_wrapper<database::DatabaseHelper> d_;
    database::DbGraph* db_graph_;
};




}
#endif // ROUTING_GRAPH_FACTORY_H