#include "routing/exception.h"

using namespace std;

namespace routing {
    RouteNotFoundException::RouteNotFoundException() : message_("Route not found.") {}

    RouteNotFoundException::RouteNotFoundException(const string & message) : message_(message) {}

    RouteNotFoundException::RouteNotFoundException(std::string && message) : message_(move(message)) {}

    RouteNotFoundException::RouteNotFoundException(const RouteNotFoundException & other) {
        message_ = other.message_;
    }

    RouteNotFoundException::RouteNotFoundException(RouteNotFoundException && other) {
        message_ = move(other.message_);
        other.message_ = string{};
    }

    RouteNotFoundException & RouteNotFoundException::operator=(const RouteNotFoundException & other) {
        RouteNotFoundException tmp{other};
        Swap(tmp);
        return *this;
    }

    RouteNotFoundException & RouteNotFoundException::operator=(RouteNotFoundException && other) {
        if (this != &other) {
            message_ = move(other.message_);
            other.message_ = string{};
        }
        return *this;
    }

    RouteNotFoundException::~RouteNotFoundException() {}

    void RouteNotFoundException::Swap(RouteNotFoundException & other) {
        swap(message_, other.message_);
    }

    const char* RouteNotFoundException::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {
        return message_.c_str();
    }
}