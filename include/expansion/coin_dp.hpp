#pragma once
//
// expansion/coin_dp.hpp
//
// Computes |w|_{G'} for elements of A_1 = N, given a macro set M, via the
// classic "minimum coins" dynamic program:
//
//   cost[0] = 0
//   cost[x] = 1 + min over generators g <= x of cost[x - g]
//
// where the available "coin denominations" are {1} u {macro lengths}.
// Exact, brute-force reference implementation -- O(x_max *
// |denominations|) -- suited to verifying the paper's theorems
// numerically on small/medium ranges, not to astronomically large x.
//
// Directly instantiates the expansion function:
//   f_{G'}(s) = sup { r : B_G(r) subset B_G'(s) }
//             = sup { r : for all x <= r, cost[x] <= s }
// computed as the largest prefix of [0, x_max] on which cost stays
// below or equal to s (see expansion/expansion_function.hpp).
//

#include <cstdint>
#include <vector>

#include "macros/macro_set.hpp"
#include "utils/types.hpp"

namespace hmcompress::expansion {

// Returns cost[0..x_max], the minimum number of generators from {1} u M
// needed to represent each integer in [0, x_max] via non-negative integer
// combinations (i.e. |x|_{G'} for A_1).
std::vector<Length> min_generator_counts(const macros::MacroSet& macro_set,
                                          Length x_max);

// Convenience: |x|_{G'} for a single x (recomputes the whole table; prefer
// min_generator_counts for repeated queries).
Length wrapped_length_single(const macros::MacroSet& macro_set, Length x);

}  // namespace hmcompress::expansion
