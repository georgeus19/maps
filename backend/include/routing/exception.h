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

    RouteNotFoundException(const RouteNotFoundException & other) = default;
    RouteNotFoundException(RouteNotFoundException && other) = default;
    RouteNotFoundException & operator=(const RouteNotFoundException & other) = default;
    RouteNotFoundException & operator=(RouteNotFoundException && other) = default;
    ~RouteNotFoundException() = default;

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

    NotImplementedException(const NotImplementedException & other) = default;
    NotImplementedException(NotImplementedException && other) = default;
    NotImplementedException & operator=(const NotImplementedException & other) = default;
    NotImplementedException & operator=(NotImplementedException && other) = default;
    ~NotImplementedException() = default;

    const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};

class VertexNotFoundException : public std::exception {
    std::string message_;
public:
    VertexNotFoundException();

    VertexNotFoundException(const std::string & message);

    VertexNotFoundException(std::string && message);

    VertexNotFoundException(const VertexNotFoundException & other) = default;
    VertexNotFoundException(VertexNotFoundException && other) = default;
    VertexNotFoundException & operator=(const VertexNotFoundException & other) = default;
    VertexNotFoundException & operator=(VertexNotFoundException && other) = default;
    ~VertexNotFoundException() = default;

    const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};

class EdgeNotFoundException : public std::exception {
    std::string message_;
public:
    EdgeNotFoundException();

    EdgeNotFoundException(const std::string & message);

    EdgeNotFoundException(std::string && message);

    EdgeNotFoundException(const EdgeNotFoundException & other) = default;
    EdgeNotFoundException(EdgeNotFoundException && other) = default;
    EdgeNotFoundException & operator=(const EdgeNotFoundException & other) = default;
    EdgeNotFoundException & operator=(EdgeNotFoundException && other) = default;
    ~EdgeNotFoundException() = default;

    void Swap(EdgeNotFoundException & other);

    const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};

class InvalidValueException : public std::exception {
    std::string message_;
public:
    InvalidValueException();
    InvalidValueException(const std::string & message);
    InvalidValueException(std::string && message);
    InvalidValueException(const InvalidValueException & other) = default;
    InvalidValueException(InvalidValueException && other) = default;
    InvalidValueException & operator=(const InvalidValueException & other) = default;
    InvalidValueException & operator=(InvalidValueException && other) = default;
    ~InvalidValueException() = default;

    const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};

class DataIndexAlreadyPresentException : public std::exception {
    std::string message_;
public:
    DataIndexAlreadyPresentException();
    DataIndexAlreadyPresentException(const std::string & message);
    DataIndexAlreadyPresentException(std::string && message);
    DataIndexAlreadyPresentException(const DataIndexAlreadyPresentException & other) = default;
    DataIndexAlreadyPresentException(DataIndexAlreadyPresentException && other) = default;
    DataIndexAlreadyPresentException & operator=(const DataIndexAlreadyPresentException & other) = default;
    DataIndexAlreadyPresentException & operator=(DataIndexAlreadyPresentException && other) = default;
    ~DataIndexAlreadyPresentException() = default;

    const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};

class ParseException : public std::exception {
    std::string message_;
public:
    ParseException();
    ParseException(const std::string & message);
    ParseException(std::string && message);
    ParseException(const ParseException & other) = default;
    ParseException(ParseException && other) = default;
    ParseException & operator=(const ParseException & other) = default;
    ParseException & operator=(ParseException && other) = default;
    ~ParseException() = default;

    const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};


}

#endif //BACKEND_EXCEPTION_H
