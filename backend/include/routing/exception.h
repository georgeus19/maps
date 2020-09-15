#ifndef BACKEND_EXCEPTION_H
#define BACKEND_EXCEPTION_H

#include <exception>
#include <string>

namespace routing {

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

        ~RouteNotFoundException();

        void Swap(RouteNotFoundException & other);

        const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
    };
}

#endif //BACKEND_EXCEPTION_H
