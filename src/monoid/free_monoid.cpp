#include "monoid/free_monoid.hpp"

#include <cmath>

namespace hmcompress::monoid {

long double FreeMonoid::sphere_size(std::uint64_t r, std::size_t n) {
    return std::pow(static_cast<long double>(n), static_cast<long double>(r));
}

long double FreeMonoid::ball_size(std::uint64_t r, std::size_t n) {
    if (n == 1) return static_cast<long double>(r) + 1.0L;
    long double n_ld = static_cast<long double>(n);
    return (std::pow(n_ld, static_cast<long double>(r) + 1.0L) - 1.0L) / (n_ld - 1.0L);
}

}  // namespace hmcompress::monoid
