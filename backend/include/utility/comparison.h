#ifndef BACKEND_UTILITY_COMPARISON_H
#define BACKEND_UTILITY_COMPARISON_H

#include <utility>

namespace utility {

template <typename Float>
bool AreEqual(Float a, Float b) {
    Float eps = 0.000001;
    return a + eps > b && a - eps < b;
}

}

#endif //BACKEND_UTILITY_COMPARISON_H
