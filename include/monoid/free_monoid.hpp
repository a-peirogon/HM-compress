#pragma once
//
// monoid/free_monoid.hpp
//
// The free (non-abelian) monoid F_n on generators {a_1, ..., a_n}. Order
// matters and there are no relations; elements are finite strings over G
// (Section 2). |w|_G is the string length.
//
// Balls grow exponentially: |B_G(r)| = (n^{r+1} - 1) / (n - 1)  (Table 1).
//

#include <cstdint>

#include "utils/types.hpp"

namespace hmcompress::monoid {

class FreeMonoid {
public:
    explicit FreeMonoid(std::size_t n) : n_(n) {}

    std::size_t rank() const { return n_; }

    // |w|_G for a word given as a sequence of generator indices.
    static Length length(const Word& w) { return w.size(); }

    // Number of distinct words of exact length r: n^r.
    // Uses long double to avoid overflow for moderate r; for exact large
    // values use utils::BigInt::Pow instead.
    static long double sphere_size(std::uint64_t r, std::size_t n);

    // |B_G(r)| = (n^{r+1} - 1) / (n - 1), the geometric series sum
    // (Table 1). Degenerates to r+1 when n == 1.
    static long double ball_size(std::uint64_t r, std::size_t n);

private:
    std::size_t n_;
};

}  // namespace hmcompress::monoid
