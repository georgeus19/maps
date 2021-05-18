#include "routing/exception.h"

using namespace std;

namespace routing {

RouteNotFoundException::RouteNotFoundException() : message_("Route not found.") {}

RouteNotFoundException::RouteNotFoundException(const string & message) : message_(message) {}

RouteNotFoundException::RouteNotFoundException(std::string && message) : message_(move(message)) {}

const char* RouteNotFoundException::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {
    return message_.c_str();
}


NotImplementedException::NotImplementedException() : message_("Route not found.") {}

NotImplementedException::NotImplementedException(const string & message) : message_(message) {}

NotImplementedException::NotImplementedException(std::string && message) : message_(move(message)) {}

const char* NotImplementedException::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {
    return message_.c_str();
}


VertexNotFoundException::VertexNotFoundException() : message_("Vertex not found.") {}

VertexNotFoundException::VertexNotFoundException(const string & message) : message_(message) {}

VertexNotFoundException::VertexNotFoundException(std::string && message) : message_(move(message)) {}

const char* VertexNotFoundException::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {
    return message_.c_str();
}


EdgeNotFoundException::EdgeNotFoundException() : message_("Edge not found.") {}

EdgeNotFoundException::EdgeNotFoundException(const string & message) : message_(message) {}

EdgeNotFoundException::EdgeNotFoundException(std::string && message) : message_(move(message)) {}

const char* EdgeNotFoundException::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {
    return message_.c_str();
}

InvalidValueException::InvalidValueException() : message_("Invalid value accessed.") {}

InvalidValueException::InvalidValueException(const string & message) : message_(message) {}

InvalidValueException::InvalidValueException(std::string && message) : message_(move(message)) {}

const char* InvalidValueException::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {
    return message_.c_str();
}

DataIndexAlreadyPresentException::DataIndexAlreadyPresentException() : message_("Data index already present.") {}

DataIndexAlreadyPresentException::DataIndexAlreadyPresentException(const string & message) : message_(message) {}

DataIndexAlreadyPresentException::DataIndexAlreadyPresentException(std::string && message) : message_(move(message)) {}

const char* DataIndexAlreadyPresentException::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {
    return message_.c_str();
}

ParseException::ParseException() : message_("Invalid value accessed.") {}

ParseException::ParseException(const string & message) : message_(message) {}

ParseException::ParseException(std::string && message) : message_(move(message)) {}

const char* ParseException::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {
    return message_.c_str();
}

InvalidArgumentException::InvalidArgumentException() : message_("Invalid value accessed.") {}

InvalidArgumentException::InvalidArgumentException(const string & message) : message_(message) {}

InvalidArgumentException::InvalidArgumentException(std::string && message) : message_(move(message)) {}

const char* InvalidArgumentException::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {
    return message_.c_str();
}


}