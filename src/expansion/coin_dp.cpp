#include "expansion/coin_dp.hpp"

#include <algorithm>

namespace hmcompress::expansion {

std::vector<Length> min_generator_counts(const macros::MacroSet& macro_set, Length x_max) {
    // Denominations: the base generator {1} plus every macro of length
    // <= x_max. Deduplicate and sort descending so the DP inner loop tries
    // the largest, most "productive" denominations first (pure optimization;
    // correctness does not depend on order).
    std::vector<Length> denominations = macro_set.lengths_up_to(x_max);
    denominations.push_back(1);
    std::sort(denominations.begin(), denominations.end());
    denominations.erase(std::unique(denominations.begin(), denominations.end()),
                         denominations.end());
    std::sort(denominations.begin(), denominations.end(), std::greater<>());

    std::vector<Length> cost(x_max + 1, kInfLength);
    cost[0] = 0;
    for (Length x = 1; x <= x_max; ++x) {
        for (Length d : denominations) {
            if (d > x) continue;
            if (cost[x - d] == kInfLength) continue;
            cost[x] = std::min(cost[x], cost[x - d] + 1);
        }
    }
    return cost;
}

Length wrapped_length_single(const macros::MacroSet& macro_set, Length x) {
    return min_generator_counts(macro_set, x).back();
}

}  // namespace hmcompress::expansion
