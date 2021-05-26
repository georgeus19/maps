#ifndef ROUTING_TABLE_NAME_REPOSITORY_H
#define ROUTING_TABLE_NAME_REPOSITORY_H

#include "routing/profile/profile.h"

#include <string>
#include <vector>

namespace routing{

class TableNameRepository{
public:
    TableNameRepository(const std::string& base_graph_table_name, const std::string& algorithm_name);

    std::string GetBaseTableName() const;

    std::string GetVerticesTable(const profile::Profile& profile) const;

    std::string GetVerticesTable(const std::string& profile_name) const;

    std::string GetEdgesTable(const profile::Profile& profile) const;

    std::string GetEdgesTable(const std::string& profile_name) const;

private:
    std::string base_graph_table_name_;
    std::string algorithm_name_;
};

TableNameRepository::TableNameRepository(const std::string& base_graph_table_name, const std::string& algorithm_name)
    : base_graph_table_name_(base_graph_table_name), algorithm_name_(algorithm_name) {}

std::string TableNameRepository::GetBaseTableName() const {
    return base_graph_table_name_;
}

std::string TableNameRepository::GetVerticesTable(const profile::Profile& profile) const {
    return GetVerticesTable(profile.GetName());
}

std::string TableNameRepository::GetVerticesTable(const std::string& profile_name) const {
    return GetEdgesTable(profile_name) + "_vertices";
}

std::string TableNameRepository::GetEdgesTable(const profile::Profile& profile) const {
    return GetEdgesTable(profile.GetName());
}

std::string TableNameRepository::GetEdgesTable(const std::string& profile_name) const {
    return algorithm_name_ + base_graph_table_name_ + profile_name;
}





}
#endif// ROUTING_TABLE_NAME_REPOSITORY_H