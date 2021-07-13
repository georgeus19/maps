#ifndef ROUTING_UTILITY_COMPARISON_H
#define ROUTING_UTILITY_COMPARISON_H

#include <utility>

namespace routing {
namespace utility {

template <typename Float>
bool AreEqual(Float a, Float b) {
    Float eps = 0.000001;
    return a + eps > b && a - eps < b;
}


}
}
#endif //ROUTING_UTILITY_COMPARISON_H