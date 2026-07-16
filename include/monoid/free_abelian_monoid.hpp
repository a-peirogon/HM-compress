#pragma once
//
// monoid/free_abelian_monoid.hpp
//
// The free abelian monoid A_n on generators {a_1, ..., a_n}. Generators
// commute, so an element is just a vector of multiplicities in N^n
// (Section 2, "elements essentially live in N^n with componentwise
// addition").
//
// |w|_G is the sum of coordinates (Section 2: "the sum of coefficients
// for A_n").
//
// A_1 = N is the primary case study in the paper (Table 1); general A_n
// results follow by taking n independent copies (see remark preceding
// Theorem 1).
//

#include <cstdint>
#include <vector>

#include "utils/types.hpp"

namespace hmcompress::monoid {

class FreeAbelianMonoid {
public:
    explicit FreeAbelianMonoid(std::size_t n) : n_(n) {}

    std::size_t rank() const { return n_; }

    // |w|_G for w given as a multiplicity vector.
    static Length length(const AbelianElement& w) {
        Length total = 0;
        for (auto x : w) total += x;
        return total;
    }

    // The generator a_i as an element of A_n (multiplicity 1 in coordinate i).
    AbelianElement generator(std::size_t i) const {
        AbelianElement e(n_, 0);
        e.at(i) = 1;
        return e;
    }

    // Ball B_G(r) cardinality: elements of A_n with |w|_G <= r.
    // This is C(r+n, n), the standard stars-and-bars count.
    static std::uint64_t ball_size(std::uint64_t r, std::size_t n);

private:
    std::size_t n_;
};

}  // namespace hmcompress::monoid
