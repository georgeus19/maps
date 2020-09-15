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
}

#endif //BACKEND_EXCEPTION_H
