#ifndef BACKEND_EDGE_DB_ROW_H
#define BACKEND_EDGE_DB_ROW_H

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
#include "database/edge_db_iterator.h"
#include "database/csv_convertor.h"
namespace database {

class DbGraph {
public:
    virtual std::string CreateEdgeSelect() const = 0;
    virtual std::unique_ptr<EdgeDbIterator> GetEdgeIterator(pqxx::result::const_iterator begin, pqxx::result::const_iterator end) const = 0;
protected:
    std::unique_ptr<EdgeDbIterator> iterator_;

    DbGraph(std::unique_ptr<EdgeDbIterator>&& iterator) : iterator_(std::move(iterator)) {}
};

class UnpreprocessedDbGraph : public DbGraph {
public:
    UnpreprocessedDbGraph() : DbGraph(std::make_unique<UnpreprocessedEdgeDbIterator>()) {}

    std::unique_ptr<EdgeDbIterator> GetEdgeIterator(pqxx::result::const_iterator begin, pqxx::result::const_iterator end) const {
        return std::make_unique<UnpreprocessedEdgeDbIterator>(begin, end);
    }

    std::string CreateEdgeSelect() const {
        return " SELECT uid, from_node, to_node, length, ST_AsText(geog) ";
    }

};

class CHPreprocessingDbGraph : public DbGraph {
public:
    CHPreprocessingDbGraph() : DbGraph(std::make_unique<CHPreprocessingEdgeDbIterator>()) {}

    std::unique_ptr<EdgeDbIterator> GetEdgeIterator(pqxx::result::const_iterator begin, pqxx::result::const_iterator end) const {
        return std::make_unique<CHPreprocessingEdgeDbIterator>(begin, end);
    }

    std::string CreateEdgeSelect() const {
        return " SELECT uid, from_node, to_node, length, ST_AsText(geog), shortcut, contracted_vertex ";
    }
};

class CHSearchDbGraph : public DbGraph {
public:
    CHSearchDbGraph() : DbGraph(std::make_unique<CHSearchEdgeDbIterator>()) {}

    std::unique_ptr<EdgeDbIterator> GetEdgeIterator(pqxx::result::const_iterator begin, pqxx::result::const_iterator end) const {
        return std::make_unique<CHSearchEdgeDbIterator>(begin, end);
    }


    std::string CreateEdgeSelect() const {
        return " SELECT uid, from_node, to_node, length, shortcut, contracted_vertex ";
    }
};




}

#endif //BACKEND_EDGE_DB_ROW_H
