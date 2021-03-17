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


NotImplementedException::NotImplementedException() : message_("Route not found.") {}

NotImplementedException::NotImplementedException(const string & message) : message_(message) {}

NotImplementedException::NotImplementedException(std::string && message) : message_(move(message)) {}

NotImplementedException::NotImplementedException(const NotImplementedException & other) {
    message_ = other.message_;
}

NotImplementedException::NotImplementedException(NotImplementedException && other) {
    message_ = move(other.message_);
    other.message_ = string{};
}

NotImplementedException & NotImplementedException::operator=(const NotImplementedException & other) {
    NotImplementedException tmp{other};
    Swap(tmp);
    return *this;
}

NotImplementedException & NotImplementedException::operator=(NotImplementedException && other) {
    if (this != &other) {
        message_ = move(other.message_);
        other.message_ = string{};
    }
    return *this;
}

NotImplementedException::~NotImplementedException() {}

void NotImplementedException::Swap(NotImplementedException & other) {
    swap(message_, other.message_);
}

const char* NotImplementedException::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {
    return message_.c_str();
}


VertexNotFoundException::VertexNotFoundException() : message_("Route not found.") {}

VertexNotFoundException::VertexNotFoundException(const string & message) : message_(message) {}

VertexNotFoundException::VertexNotFoundException(std::string && message) : message_(move(message)) {}

VertexNotFoundException::VertexNotFoundException(const VertexNotFoundException & other) {
    message_ = other.message_;
}

VertexNotFoundException::VertexNotFoundException(VertexNotFoundException && other) {
    message_ = move(other.message_);
    other.message_ = string{};
}

VertexNotFoundException & VertexNotFoundException::operator=(const VertexNotFoundException & other) {
    VertexNotFoundException tmp{other};
    Swap(tmp);
    return *this;
}

VertexNotFoundException & VertexNotFoundException::operator=(VertexNotFoundException && other) {
    if (this != &other) {
        message_ = move(other.message_);
        other.message_ = string{};
    }
    return *this;
}

VertexNotFoundException::~VertexNotFoundException() {}

void VertexNotFoundException::Swap(VertexNotFoundException & other) {
    swap(message_, other.message_);
}

const char* VertexNotFoundException::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {
    return message_.c_str();
}


EdgeNotFoundException::EdgeNotFoundException() : message_("Route not found.") {}

EdgeNotFoundException::EdgeNotFoundException(const string & message) : message_(message) {}

EdgeNotFoundException::EdgeNotFoundException(std::string && message) : message_(move(message)) {}

EdgeNotFoundException::EdgeNotFoundException(const EdgeNotFoundException & other) {
    message_ = other.message_;
}

EdgeNotFoundException::EdgeNotFoundException(EdgeNotFoundException && other) {
    message_ = move(other.message_);
    other.message_ = string{};
}

EdgeNotFoundException & EdgeNotFoundException::operator=(const EdgeNotFoundException & other) {
    EdgeNotFoundException tmp{other};
    Swap(tmp);
    return *this;
}

EdgeNotFoundException & EdgeNotFoundException::operator=(EdgeNotFoundException && other) {
    if (this != &other) {
        message_ = move(other.message_);
        other.message_ = string{};
    }
    return *this;
}

EdgeNotFoundException::~EdgeNotFoundException() {}

void EdgeNotFoundException::Swap(EdgeNotFoundException & other) {
    swap(message_, other.message_);
}

const char* EdgeNotFoundException::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {
    return message_.c_str();
}




}