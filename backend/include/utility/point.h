#ifndef BACKEND_POINT_H
#define BACKEND_POINT_H

#include <utility>

namespace utility {

    /**
     * Point represent geographical location on Earth.
     */
    struct Point {
        double lon_;
        double lat_;

        Point(double lon, double lat);

        Point(const Point & other);

        Point(Point && other);

        Point& operator=(const Point & other);

        Point& operator=(Point && other);

        ~Point();

        void Swap(Point & other);
    };
}

#endif //BACKEND_POINT_H
