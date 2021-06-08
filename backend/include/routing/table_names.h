#ifndef ROUTING_TABLE_NAME_REPOSITORY_H
#define ROUTING_TABLE_NAME_REPOSITORY_H

#include "routing/profile/profile.h"
#include "routing/constants.h"
#include "routing/exception.h"

#include <string>
#include <vector>

namespace routing{

class TableNames{
public:
    TableNames() {}

    TableNames(TableNames&& other) = delete;
    TableNames(const TableNames& other) = delete;
    TableNames& operator=(TableNames&& other) = delete;
    TableNames& operator=(const TableNames& other) = delete;
    ~TableNames() = default;

    virtual const std::string& GetBaseTableName() const = 0;

    virtual const std::string& GetEdgesTable() const = 0;

    virtual const std::string& GetVerticesTable() const = 0;

    virtual const std::string& GetIndexTablePrefix() const = 0;
};

class DijkstraTableNames : public TableNames{
public:
    DijkstraTableNames(const std::string& base_graph_table) : base_graph_table_(base_graph_table), index_table_prefix_() {}

    const std::string& GetBaseTableName() const override {
        return base_graph_table_;
    }

    const std::string& GetEdgesTable() const override {
        return base_graph_table_;
    }

    const std::string& GetVerticesTable() const override {
        throw NotImplementedException{"Dijkstra does not have any vertices table!"};
    }

    const std::string& GetIndexTablePrefix() const override {
        return index_table_prefix_;
    }
private:
    std::string base_graph_table_;
    std::string index_table_prefix_;
};

class CHTableNames : public TableNames{
public:
    CHTableNames(const std::string& base_graph_table, const profile::Profile& profile, const std::string& table_prefix)
        : base_graph_table_(base_graph_table), edges_table_(table_prefix + Constants::AlgorithmNames::kContractionHierarchies + base_graph_table + profile.GetName()), 
            vertices_table_(edges_table_ + "vertices_"), index_table_prefix_(edges_table_) {}

    const std::string& GetBaseTableName() const override {
        return base_graph_table_;
    }

    const std::string& GetEdgesTable() const override {
        return edges_table_;
    }

    const std::string& GetVerticesTable() const override {
        return vertices_table_;
    }

    const std::string& GetIndexTablePrefix() const override {
        return index_table_prefix_;
    }

private:
    std::string base_graph_table_;
    std::string edges_table_;
    std::string vertices_table_;
    std::string index_table_prefix_;
};



}
#endif// ROUTING_TABLE_NAME_REPOSITORY_H