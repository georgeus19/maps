#ifndef ROUTING_TABLE_NAME_REPOSITORY_H
#define ROUTING_TABLE_NAME_REPOSITORY_H

#include "routing/profile/profile.h"

#include <string>
#include <vector>

namespace routing{

class TableNameRepository{
public:
    TableNameRepository(const std::string& base_graph_table_name, const std::string& algorithm_name);

    std::string GetVerticesTable(const profile::Profile& profile);

    std::string GetEdgesTable(const profile::Profile& profile);

private:
    std::string base_graph_table_name_;
    std::string algorithm_name_;
};

TableNameRepository::TableNameRepository(const std::string& base_graph_table_name, const std::string& algorithm_name)
    : base_graph_table_name_(base_graph_table_name), algorithm_name_(algorithm_name) {}

std::string TableNameRepository::GetVerticesTable(const profile::Profile& profile) {
    return GetEdgesTable(profile) + "_vertices";
}

std::string TableNameRepository::GetEdgesTable(const profile::Profile& profile) {
    return algorithm_name_ + base_graph_table_name_ + profile.GetName();
}






}
#endif// ROUTING_TABLE_NAME_REPOSITORY_H