
#include "utility/point.h"
namespace utility {
    Point::Point(double lon, double lat) : lon_(lon), lat_(lat) {}

    Point::Point(const Point & other) {
        lon_ = other.lon_;
        lat_ = other.lat_;
    }

    Point::Point(Point && other) {
        lon_ = other.lon_;
        lat_ = other.lat_;
    }

    Point& Point::operator=(const Point & other) {
        Point tmp{other};
        Swap(tmp);
        return *this;
    }

    Point& Point::operator=(Point && other) {
        if (this != &other) {
            lon_ = other.lon_;
            lat_ = other.lat_;
        }
        return *this;
    }

    Point::~Point() {}

    void Point::Swap(Point & other) {
        std::swap(lon_, other.lon_);
        std::swap(lat_, other.lat_);
    }
}