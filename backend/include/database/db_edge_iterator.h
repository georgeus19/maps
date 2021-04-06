#ifndef BACKEND_DB_EDGE_ITERATOR_H
#define BACKEND_DB_EDGE_ITERATOR_H
#include <string>
#include <string_view>
#include <pqxx/pqxx>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <numeric>
#include <fstream>
#include <exception>
#include <functional>
#include <utility>
#include "utility/point.h"
#include "database/csv_convertor.h"
#include "database/db_edge_iterator.h"

namespace database {

/**
 * DbEdgeIterator provides an interface to read a row of data representing graph edge from database
 * that can be used to create instance of Edge class so that
 * Edge itself does not need to know the detail interface of
 * library pqxx.
 */
class DbEdgeIterator {
public:

    inline void Inc() {
        ++it_;
    }

    inline bool IsEnd() const {
        return it_ == end_;
    }

    virtual uint64_t GetUid() const = 0;
    virtual uint64_t GetFrom() const = 0;
    virtual uint64_t GetTo() const = 0;
    virtual double GetLength() const = 0;
    virtual bool GetShortcut() const = 0;
    virtual uint64_t GetContractedVertex() const = 0;
    virtual std::string GetGeography() const = 0;
protected:
    pqxx::result::const_iterator it_;
    pqxx::result::const_iterator end_;

    DbEdgeIterator(pqxx::result::const_iterator begin, pqxx::result::const_iterator end) : it_(begin), end_(end) {}
};

/**
 * UnpreprocessedDbEdgeRow provides an interface to read a row of data representing graph edge from database
 * that can be used to create instance of Edge class so that
 * Edge itself does not need to know the detail interface of
 * library pqxx.
 */
class UnpreprocessedDbEdgeIterator : public DbEdgeIterator {
public:
    UnpreprocessedDbEdgeIterator(pqxx::result::const_iterator begin, pqxx::result::const_iterator end)
        : DbEdgeIterator(begin, end) {}


    uint64_t GetUid() const override {
        return it_[0].as<uint64_t>();
    }
   
    std::string GetGeography() const override {
        return it_[1].as<std::string>();
    }

    uint64_t GetFrom() const override {
        return it_[2].as<uint64_t>();
    }

    uint64_t GetTo() const override {
        return it_[3].as<uint64_t>();
    }

    double GetLength() const override {
        return it_[4].as<double>();
    }

    bool GetShortcut() const override {
        return false;
    }

    uint64_t GetContractedVertex() const override {
        return 0;
    }


};


/**
 * CHPreprocessingDbEdgeRow provides an interface to read a row of data representing graph edge from database
 * that can be used to create instance of Edge class so that
 * Edge itself does not need to know the detail interface of
 * library pqxx.
 */
class CHDbEdgeIterator : public DbEdgeIterator {
public:
    CHDbEdgeIterator(pqxx::result::const_iterator begin, pqxx::result::const_iterator end)
        : DbEdgeIterator(begin, end) {}

    uint64_t GetUid() const override {
        return it_[0].as<uint64_t>();
    }
   
    std::string GetGeography() const override {
        return it_[1].as<std::string>();
    }

    uint64_t GetFrom() const override {
        return it_[2].as<uint64_t>();
    }

    uint64_t GetTo() const override {
        return it_[3].as<uint64_t>();
    }

    double GetLength() const override {
        return it_[4].as<double>();
    }

    bool GetShortcut() const override {
        return it_[5].as<bool>();
    }

    uint64_t GetContractedVertex() const override {
        return it_[6].as<uint64_t>();
    }

};



}
#endif //BACKEND_DB_EDGE_ITERATOR_H