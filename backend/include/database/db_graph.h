#ifndef BACKEND_DB_GRAPH_H
#define BACKEND_DB_GRAPH_H

#include <vector>
#include <set>
#include <queue>
#include <iostream>
#include <fstream>
#include <cassert>
#include <functional>
#include <string_view>
#include <string>
#include <pqxx/pqxx>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <numeric>
#include <fstream>
#include <exception>
#include <functional>
#include <utility>
#include <memory>
#include "utility/point.h"
#include "database/db_edge_iterator.h"
#include "database/csv_convertor.h"
namespace database {

class DbGraph {
public:
    DbGraph() {}
    virtual std::string GetEdgeSelect(const std::string& table_alias = "") const = 0;
    virtual std::unique_ptr<DbEdgeIterator> GetEdgeIterator(pqxx::result::const_iterator begin, pqxx::result::const_iterator end) const = 0;
    virtual std::string GetCreateGraphTable(const std::string& basic_graph_table, const std::string& new_table) const = 0;
    virtual std::string GetEndpointEdgeCondition(const std::string& table_alias = "") const = 0;
    virtual std::string GetVertexSelect(const std::string& table_alias = "") const = 0;
 
protected:
    std::string GetAliasExpression(const std::string& alias) const {
        if (alias.empty()) {
            return alias;
        } else {
            return alias + ".";
        }
    }
};

class UnpreprocessedDbGraph : public DbGraph {
public:
    UnpreprocessedDbGraph() : DbGraph() {}

    std::string GetEdgeSelect(const std::string& table_alias = "") const override {
        std::string alias_expression = GetAliasExpression(table_alias);
        return " SELECT " + alias_expression + "uid, " \
            " ST_AsText(" + alias_expression + "geog), "
            + alias_expression + "from_node, "
            + alias_expression + "to_node, "
            + alias_expression + "length ";
    }


    std::unique_ptr<DbEdgeIterator> GetEdgeIterator(pqxx::result::const_iterator begin, pqxx::result::const_iterator end) const override {
        return std::make_unique<UnpreprocessedDbEdgeIterator>(begin, end);
    }

    std::string GetCreateGraphTable(const std::string& basic_graph_table, const std::string& new_table) const override {
        return "CREATE TABLE " + new_table + " AS SELECT * FROM " + basic_graph_table + ";";
    }

    std::string GetEndpointEdgeCondition(const std::string& table_alias = "") const override {
        return " true = true ";
    }

    std::string GetVertexSelect(const std::string& table_alias = "") const override {
        return " SELECT * ";
    }

};

class CHDbGraph : public DbGraph {
public:
    CHDbGraph() : DbGraph() {}

    std::string GetEdgeSelect(const std::string& table_alias = "") const override {
        std::string alias_expression = GetAliasExpression(table_alias);
        return " SELECT " + alias_expression + "uid, " \
        " ST_AsText(" + alias_expression + "geog), "
        + alias_expression + "from_node, "
        + alias_expression + "to_node, "
        + alias_expression + "length, "
        + alias_expression + "shortcut, "
        + alias_expression + "contracted_vertex ";
    }

    std::string GetCreateGraphTable(const std::string& basic_graph_table, const std::string& new_table) const override {
        std::string create_table_sql = "CREATE TABLE " + new_table + " ( " \
				"osm_id BIGINT NOT NULL, " \
				"uid BIGINT PRIMARY KEY, " \
				"geog geography(LINESTRING) NOT NULL, " \
				"from_node BIGINT NOT NULL, " \
				"to_node BIGINT NOT NULL, " \
                "length DOUBLE PRECISION NOT NULL, "
				"shortcut BOOLEAN NOT NULL, " \
				"contracted_vertex BIGINT NOT NULL); ";
        std::string insert_basic_graph_sql = "INSERT INTO " + new_table + "(osm_id, uid, geog, from_node, to_node, length, shortcut, contracted_vertex) " \
                "( " \
                "SELECT osm_id, uid, geog, from_node, to_node, length, false, 0 FROM "+ basic_graph_table + " " \
                "); ";
        return create_table_sql + insert_basic_graph_sql;
    }

    std::unique_ptr<DbEdgeIterator> GetEdgeIterator(pqxx::result::const_iterator begin, pqxx::result::const_iterator end) const override {
        return std::make_unique<CHDbEdgeIterator>(begin, end);
    }

    std::string GetEndpointEdgeCondition(const std::string& table_alias = "") const override {
        std::string alias_expression = GetAliasExpression(table_alias);
        return " " + alias_expression + "shortcut = false ";
    }

    std::string GetVertexSelect(const std::string& table_alias = "") const override {
        std::string alias_expression = GetAliasExpression(table_alias);
        return " SELECT "
            + alias_expression + "osm_id, "
            + alias_expression + "ordering_rank ";
    }
};






}

#endif //BACKEND_DB_GRAPH_H
