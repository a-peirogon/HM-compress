#pragma once
//
// expansion/expansion_function.hpp
//
// The expansion function, Section 2:
//
//   f_{G'}(s) = sup { r in N : B_G(r) subset B_G'(s) }
//
// i.e. the largest G-radius fully covered by the G'-ball of radius s.
//
// Given a precomputed cost table cost[0..x_max] (from coin_dp or
// segmentation_dp, where cost[x] = |x|_{G'}), f_{G'}(s) is simply the
// length of the longest prefix [0, r] on which cost[x] <= s for all
// x <= r -- i.e. one less than the first index where cost exceeds s.
//
// Enables numerical reproduction of the growth rates claimed in Table 1
// (Theta(b^{s/(n(b-1))}) for place notation, Theta(s) for finite macro
// sets, etc.) by plotting f_{G'}(s) against s on the appropriate scale.
//

#include <cstdint>
#include <vector>

#include "utils/types.hpp"

namespace hmcompress::expansion {

// Given cost[0..x_max] (cost[x] = |x|_{G'}), returns f_{G'}(s): the
// largest r such that cost[x] <= s for every x in [0, r].
// If no such r exists within [0, x_max] (i.e. cost[0] > s, impossible
// since cost[0] = 0), returns 0. If cost never exceeds s within the
// table's range, returns x_max (a lower bound -- the true f_{G'}(s) may be
// larger; enlarge x_max to refine).
Length expansion_from_cost_table(const std::vector<Length>& cost, Length s);

// Convenience: computes f_{G'}(s) for every s in [0, s_max], given a cost
// table. O(x_max) total (single pass), since f_{G'} is monotonic in s.
std::vector<Length> expansion_curve(const std::vector<Length>& cost, Length s_max);

}  // namespace hmcompress::expansion
