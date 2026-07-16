#include "monoid/free_abelian_monoid.hpp"

namespace hmcompress::monoid {

std::uint64_t FreeAbelianMonoid::ball_size(std::uint64_t r, std::size_t n) {
    // C(r+n, n) computed via multiplicative formula to avoid factorial
    // overflow: product_{i=1}^{n} (r+i) / i.
    long double result = 1.0L;
    for (std::size_t i = 1; i <= n; ++i) {
        result *= static_cast<long double>(r + i) / static_cast<long double>(i);
    }
    return static_cast<std::uint64_t>(result + 0.5L);
}

}  // namespace hmcompress::monoid
