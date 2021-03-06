#ifndef ROUTING_DATABASE_DB_GRAPH_H
#define ROUTING_DATABASE_DB_GRAPH_H

#include "routing/utility/point.h"
#include "routing/database/db_edge_iterator.h"
#include "routing/database/csv_convertor.h"
#include "routing/types.h"

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

namespace routing {
namespace database {

/**
 * DbGraph is an abstract class declaring methods for loading and saving different types of graphs to/from database.
 */
class DbGraph {
public:
    DbGraph() {}
    virtual std::string GetEdgeSelect(const std::string& table_alias = "") const = 0;
    virtual std::unique_ptr<DbEdgeIterator> GetEdgeIterator(pqxx::result::const_iterator begin, pqxx::result::const_iterator end) const = 0;
    virtual std::string GetCreateGraphTable(const std::string& table_name) const = 0;
    virtual std::string GetEndpointEdgeCondition(const std::string& table_alias = "") const = 0;
    virtual std::string GetVertexSelect(const std::string& table_alias = "") const = 0;
    virtual std::string GetCreateVertexTable(const std::string& table_name) const = 0;
 
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
            + alias_expression + "undirected, "
            + alias_expression + "length ";
    }


    std::unique_ptr<DbEdgeIterator> GetEdgeIterator(pqxx::result::const_iterator begin, pqxx::result::const_iterator end) const override {
        return std::make_unique<UnpreprocessedDbEdgeIterator>(begin, end);
    }

    std::string GetCreateGraphTable(const std::string& table_name) const override {
        std::string create_table = "CREATE TABLE " + table_name + " ( " \
				"osm_id BIGINT NOT NULL, " \
				"uid INTEGER PRIMARY KEY, " \
				"geog geography(LINESTRING), " \
				"from_node INTEGER NOT NULL, " \
				"to_node INTEGER NOT NULL, " \
                "undirected BOOLEAN NOT NULL, " \
                "length REAL NOT NULL); ";
        return create_table;
    }

    std::string GetEndpointEdgeCondition(const std::string& table_alias = "") const override {
        return " true = true ";
    }

    std::string GetVertexSelect(const std::string& table_alias = "") const override {
        return " SELECT * ";
    }

    std::string GetCreateVertexTable(const std::string& table_name) const override {
        std::string create_table = "CREATE TABLE " + table_name + "("  \
            "uid INTEGER PRIMARY KEY); ";
        return create_table;
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
        + alias_expression + "undirected, "
        + alias_expression + "length, "
        + alias_expression + "shortcut, "
        + alias_expression + "contracted_vertex ";
    }

    std::string GetCreateGraphTable(const std::string& table_name) const override {
        std::string create_table = "CREATE TABLE " + table_name + " ( " \
				"osm_id BIGINT NOT NULL, " \
				"uid INTEGER PRIMARY KEY, " \
				"geog geography(LINESTRING), " \
				"from_node INTEGER NOT NULL, " \
				"to_node INTEGER NOT NULL, " \
                "undirected BOOLEAN NOT NULL, " \
                "length REAL NOT NULL, " \
				"shortcut BOOLEAN NOT NULL, " \
				"contracted_vertex INTEGER NOT NULL); ";
        return create_table;
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

    std::string GetCreateVertexTable(const std::string& table_name) const override {
        std::string create_table = "CREATE TABLE " + table_name + "("  \
            "uid INTEGER PRIMARY KEY, " \
            "ordering_rank INTEGER NOT NULL); ";
        return create_table;
    }
};






}
}
#endif //ROUTING_DATABASE_DB_GRAPH_H
