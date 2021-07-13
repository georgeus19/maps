#ifndef ROUTING_UTILITY_POINT_H
#define ROUTING_UTILITY_POINT_H

#include <utility>

namespace routing {
namespace utility {

/**
 * Point represent geographical location on Earth.
 */
struct Point {
    double lon_;
    double lat_;

    Point(double lon, double lat);

    Point(const Point & other) = default;

    Point(Point && other) = default;;

    Point& operator=(const Point & other) = default;;

    Point& operator=(Point && other) = default;

    ~Point() = default;

    void Swap(Point & other);
};


}
}
#endif //ROUTING_UTILITY_POINT_H