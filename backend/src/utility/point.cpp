
#include "utility/point.h"
namespace utility {
    Point::Point(double lon, double lat) : lon_(lon), lat_(lat) {}

    void Point::Swap(Point & other) {
        std::swap(lon_, other.lon_);
        std::swap(lat_, other.lat_);
    }
}