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

    RouteNotFoundException(const RouteNotFoundException & other);

    RouteNotFoundException(RouteNotFoundException && other);

    RouteNotFoundException & operator=(const RouteNotFoundException & other);

    RouteNotFoundException & operator=(RouteNotFoundException && other);

    ~RouteNotFoundException() override;

    void Swap(RouteNotFoundException & other);

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

    NotImplementedException(const NotImplementedException & other);

    NotImplementedException(NotImplementedException && other);

    NotImplementedException & operator=(const NotImplementedException & other);

    NotImplementedException & operator=(NotImplementedException && other);

    ~NotImplementedException() override;

    void Swap(NotImplementedException & other);

    const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};

class VertexNotFoundException : public std::exception {
    std::string message_;
public:
    VertexNotFoundException();

    VertexNotFoundException(const std::string & message);

    VertexNotFoundException(std::string && message);

    VertexNotFoundException(const VertexNotFoundException & other);

    VertexNotFoundException(VertexNotFoundException && other);

    VertexNotFoundException & operator=(const VertexNotFoundException & other);

    VertexNotFoundException & operator=(VertexNotFoundException && other);

    ~VertexNotFoundException() override;

    void Swap(VertexNotFoundException & other);

    const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};

class EdgeNotFoundException : public std::exception {
    std::string message_;
public:
    EdgeNotFoundException();

    EdgeNotFoundException(const std::string & message);

    EdgeNotFoundException(std::string && message);

    EdgeNotFoundException(const EdgeNotFoundException & other);

    EdgeNotFoundException(EdgeNotFoundException && other);

    EdgeNotFoundException & operator=(const EdgeNotFoundException & other);

    EdgeNotFoundException & operator=(EdgeNotFoundException && other);

    ~EdgeNotFoundException() override;

    void Swap(EdgeNotFoundException & other);

    const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};
}

#endif //BACKEND_EXCEPTION_H
