#ifndef ROUTING_DATABASE_DB_EDGE_ITERATOR_H
#define ROUTING_DATABASE_DB_EDGE_ITERATOR_H
#include "routing/utility/point.h"
#include "routing/database/csv_convertor.h"
#include "routing/types.h"

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

namespace routing {
namespace database {

/**
 * DbEdgeIterator provides an interface to read a row of data representing graph edge from database
 * that can be used to create instance of Edge class so that
 * Edge itself does not need to know the detail interface of
 * library pqxx.
 */
class DbEdgeIterator {
public:

    virtual ~DbEdgeIterator() = default;

    inline void Inc() {
        ++it_;
    }

    inline bool IsEnd() const {
        return it_ == end_;
    }

    virtual unsigned_id_type GetUid() const = 0;
    virtual unsigned_id_type GetFrom() const = 0;
    virtual unsigned_id_type GetTo() const = 0;
    virtual float GetLength() const = 0;
    virtual bool GetUndirected() const = 0;
    virtual bool GetShortcut() const = 0;
    virtual unsigned_id_type GetContractedVertex() const = 0;
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


    unsigned_id_type GetUid() const override {
        return it_[0].as<unsigned_id_type>();
    }
   
    std::string GetGeography() const override {
        return it_[1].as<std::string>();
    }

    unsigned_id_type GetFrom() const override {
        return it_[2].as<unsigned_id_type>();
    }

    unsigned_id_type GetTo() const override {
        return it_[3].as<unsigned_id_type>();
    }

    bool GetUndirected() const override {
        return it_[4].as<bool>();
    }

    float GetLength() const override {
        return it_[5].as<float>();
    }

    bool GetShortcut() const override {
        return false;
    }

    unsigned_id_type GetContractedVertex() const override {
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

    unsigned_id_type GetUid() const override {
        return it_[0].as<unsigned_id_type>();
    }
   
    std::string GetGeography() const override {
        return it_[1].as<std::string>();
    }

    unsigned_id_type GetFrom() const override {
        return it_[2].as<unsigned_id_type>();
    }

    unsigned_id_type GetTo() const override {
        return it_[3].as<unsigned_id_type>();
    }

    bool GetUndirected() const override {
        return it_[4].as<bool>();
    }

    float GetLength() const override {
        return it_[5].as<float>();
    }

    bool GetShortcut() const override {
        return it_[6].as<bool>();
    }

    unsigned_id_type GetContractedVertex() const override {
        return it_[7].as<unsigned_id_type>();
    }


};



}
}
#endif //ROUTING_DATABASE_DB_EDGE_ITERATOR_H