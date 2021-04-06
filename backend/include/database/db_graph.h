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
    virtual std::string GetEdgeSelect() const = 0;
    virtual std::unique_ptr<DbEdgeIterator> GetEdgeIterator(pqxx::result::const_iterator begin, pqxx::result::const_iterator end) const = 0;
    virtual std::string GetCreateGraphTable(const std::string& basic_graph_table, const std::string& new_table) const = 0;
};

class UnpreprocessedDbGraph : public DbGraph {
public:
    UnpreprocessedDbGraph() : DbGraph() {}

    std::string GetEdgeSelect() const override {
        return " SELECT uid, ST_AsText(geog), from_node, to_node, length ";
    }


    std::unique_ptr<DbEdgeIterator> GetEdgeIterator(pqxx::result::const_iterator begin, pqxx::result::const_iterator end) const override {
        return std::make_unique<UnpreprocessedDbEdgeIterator>(begin, end);
    }

    std::string GetCreateGraphTable(const std::string& basic_graph_table, const std::string& new_table) const override {
        return "CREATE TABLE " + new_table + " AS SELECT * FROM " + basic_graph_table + ";";
    }

};

class CHDbGraph : public DbGraph {
public:
    CHDbGraph() : DbGraph() {}

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

    std::string GetEdgeSelect() const override {
        return " SELECT uid, ST_AsText(geog), from_node, to_node, length, shortcut, contracted_vertex ";
    }
};






}

#endif //BACKEND_DB_GRAPH_H
