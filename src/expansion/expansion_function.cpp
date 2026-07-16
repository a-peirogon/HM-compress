#include "expansion/expansion_function.hpp"

#include <algorithm>

namespace hmcompress::expansion {

Length expansion_from_cost_table(const std::vector<Length>& cost, Length s) {
    if (cost.empty()) return 0;
    Length r = 0;
    for (std::size_t x = 0; x < cost.size(); ++x) {
        if (cost[x] > s) {
            // First x where the cost exceeds s: everything strictly below
            // is covered, so f_{G'}(s) = x - 1 (as an index, x-1 == largest
            // covered r since indices are exactly the integers of A_1).
            return static_cast<Length>(x) - 1;
        }
        r = static_cast<Length>(x);
    }
    // Never exceeded s within the table: return the table's range as a
    // lower bound on the true (possibly larger) f_{G'}(s).
    return r;
}

std::vector<Length> expansion_curve(const std::vector<Length>& cost, Length s_max) {
    std::vector<Length> f(s_max + 1, 0);
    // For each x, the minimal s for which x is covered equals
    // max(cost[0..x]) (since B_G(r) subset B_G'(s) requires every x <= r
    // to satisfy cost[x] <= s). f_{G'}(s) is then the largest r such that
    // running_max[r] <= s: the inverse of a monotonic non-decreasing
    // sequence, computed with a two-pointer scan in O(x_max + s_max).
    std::vector<Length> running_max(cost.size());
    Length cur_max = 0;
    for (std::size_t x = 0; x < cost.size(); ++x) {
        cur_max = std::max(cur_max, cost[x]);
        running_max[x] = cur_max;
    }
    std::size_t x_ptr = 0;
    for (Length s = 0; s <= s_max; ++s) {
        while (x_ptr < running_max.size() && running_max[x_ptr] <= s) ++x_ptr;
        f[s] = (x_ptr == 0) ? 0 : static_cast<Length>(x_ptr - 1);
    }
    return f;
}

}  // namespace hmcompress::expansion
