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
};

class UnpreprocessedDbGraph : public DbGraph {
public:
    UnpreprocessedDbGraph() : DbGraph() {}

    std::unique_ptr<DbEdgeIterator> GetEdgeIterator(pqxx::result::const_iterator begin, pqxx::result::const_iterator end) const {
        return std::make_unique<UnpreprocessedDbEdgeIterator>(begin, end);
    }

    std::string GetEdgeSelect() const {
        return " SELECT uid, from_node, to_node, length, ST_AsText(geog) ";
    }

};

class CHPreprocessingDbGraph : public DbGraph {
public:
    CHPreprocessingDbGraph() : DbGraph() {}

    std::unique_ptr<DbEdgeIterator> GetEdgeIterator(pqxx::result::const_iterator begin, pqxx::result::const_iterator end) const {
        return std::make_unique<CHPreprocessingDbEdgeIterator>(begin, end);
    }

    std::string GetEdgeSelect() const {
        return " SELECT uid, from_node, to_node, length, ST_AsText(geog), shortcut, contracted_vertex ";
    }
};

class CHSearchDbGraph : public DbGraph {
public:
    CHSearchDbGraph() : DbGraph() {}

    std::unique_ptr<DbEdgeIterator> GetEdgeIterator(pqxx::result::const_iterator begin, pqxx::result::const_iterator end) const {
        return std::make_unique<CHSearchDbEdgeIterator>(begin, end);
    }


    std::string GetEdgeSelect() const {
        return " SELECT uid, from_node, to_node, length, shortcut, contracted_vertex ";
    }
};




}

#endif //BACKEND_DB_GRAPH_H
