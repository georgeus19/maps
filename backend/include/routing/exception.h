#ifndef BACKEND_EXCEPTION_H
#define BACKEND_EXCEPTION_H

#include <exception>
#include <string>

namespace routing {

/**
 * RouteNotFoundException is thrown when it is impossible to find the best route.
 */
class RouteNotFoundException : public std::exception {
    std::string message_;
public:
    RouteNotFoundException();
    RouteNotFoundException(const std::string & message);
    RouteNotFoundException(std::string && message);

    const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};

/**
 * NotImplementedException is thrown when it is impossible to find the best route.
 */
class NotImplementedException : public std::exception {
    std::string message_;
public:
    NotImplementedException();
    NotImplementedException(const std::string & message);
    NotImplementedException(std::string && message);

    const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};

class VertexNotFoundException : public std::exception {
    std::string message_;
public:
    VertexNotFoundException();
    VertexNotFoundException(const std::string & message);
    VertexNotFoundException(std::string && message);

    const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};

class EdgeNotFoundException : public std::exception {
    std::string message_;
public:
    EdgeNotFoundException();
    EdgeNotFoundException(const std::string & message);
    EdgeNotFoundException(std::string && message);

    void Swap(EdgeNotFoundException & other);

    const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};

class InvalidValueException : public std::exception {
    std::string message_;
public:
    InvalidValueException();
    InvalidValueException(const std::string & message);
    InvalidValueException(std::string && message);

    const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};

class DataIndexAlreadyPresentException : public std::exception {
    std::string message_;
public:
    DataIndexAlreadyPresentException();
    DataIndexAlreadyPresentException(const std::string & message);
    DataIndexAlreadyPresentException(std::string && message);

    const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};

class ParseException : public std::exception {
    std::string message_;
public:
    ParseException();
    ParseException(const std::string & message);
    ParseException(std::string && message);

    const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};

class InvalidArgumentException : public std::exception {
    std::string message_;
public:
    InvalidArgumentException();
    InvalidArgumentException(const std::string& message);
    InvalidArgumentException(std::string&& message);

    const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};




}
#endif //BACKEND_EXCEPTION_H