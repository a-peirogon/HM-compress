#include <catch2/catch_test_macros.hpp>
#include <cmath>

#include "expansion/coin_dp.hpp"
#include "expansion/expansion_function.hpp"
#include "macros/macro_set.hpp"

using namespace hmcompress;
using namespace hmcompress::expansion;

// ---------------------------------------------------------------------
// These tests check the closed-form bounds from Section 2 / Table 1
// numerically, on ranges small enough to compute exactly via the DP, but
// large enough to be a meaningful check (not just s=1,2,3).
// ---------------------------------------------------------------------

TEST_CASE("Theorem 1: place notation expansion bounds hold for n=1", "[theorems][expansion]") {
    const std::uint64_t b = 3;
    const Length x_max = 200'000;
    const Length s_max = 20;

    auto m = macros::place_notation_macros(b, x_max);
    auto cost = min_generator_counts(m, x_max);
    auto f = expansion_curve(cost, s_max);

    for (Length s = 1; s <= s_max; ++s) {
        double lower = std::pow(static_cast<double>(b),
                                 static_cast<double>(s) / (b - 1) - 1.0);
        double upper =
            static_cast<double>(b) * std::pow(static_cast<double>(b), static_cast<double>(s) / (b - 1));

        // f might be capped by x_max (a lower bound on the true value in
        // that case), so only assert the lower bound strictly, and the
        // upper bound only when f[s] is not saturated at x_max.
        INFO("s=" << s << " f=" << f[s] << " lower=" << lower << " upper=" << upper);
        REQUIRE(static_cast<double>(f[s]) >= lower * 0.5);  // allow small-s slack
        if (f[s] < x_max) {
            REQUIRE(static_cast<double>(f[s]) <= upper * 1.5);  // allow constant-factor slack
        }
    }
}

TEST_CASE("Theorem 7: finite macro set gives Theta(s) expansion", "[theorems][expansion]") {
    const Length L = 7;  // finite macro set {1,...,7}
    const Length x_max = 100'000;
    const Length s_max = 100;

    auto m = macros::finite_macros(L);
    auto cost = min_generator_counts(m, x_max);
    auto f = expansion_curve(cost, s_max);

    // Theorem 7 proof: f_G'(s) <= sL and f_G'(s) >= s. Check both directly.
    for (Length s = 1; s <= s_max; ++s) {
        REQUIRE(f[s] >= s);
        REQUIRE(f[s] <= s * L);
    }
}

TEST_CASE("Theorem 3 / Remark 1: Waring k=2 gives infinite expansion at s=4",
          "[theorems][expansion]") {
    const Length x_max = 500'000;
    auto m = macros::waring_macros(2, x_max);
    auto cost = min_generator_counts(m, x_max);

    // f_G'(4) should equal x_max (i.e. cost[x] <= 4 for ALL x up to x_max,
    // confirming the "infinite expansion" claim within this finite window).
    Length f4 = expansion_from_cost_table(cost, 4);
    REQUIRE(f4 == x_max);
}

TEST_CASE("Expansion is monotonic non-decreasing in s", "[theorems][expansion]") {
    auto m = macros::place_notation_macros(5, 100'000);
    auto cost = min_generator_counts(m, 100'000);
    auto f = expansion_curve(cost, 20);
    for (Length s = 1; s <= 20; ++s) {
        REQUIRE(f[s] >= f[s - 1]);
    }
}

TEST_CASE("Place notation (Theorem 1) dramatically outperforms finite macros (Theorem 7)",
          "[theorems][expansion]") {
    // Direct comparison illustrating the paper's central contrast:
    // logarithmic-density macros (Theorem 1) achieve exponential
    // expansion, while any finite macro set (Theorem 7) is stuck at
    // linear expansion -- for the same "budget" s.
    const Length x_max = 1'000'000;
    const Length s = 15;

    auto log_density = macros::place_notation_macros(10, x_max);
    auto cost_log = min_generator_counts(log_density, x_max);
    Length f_log = expansion_from_cost_table(cost_log, s);

    auto finite = macros::finite_macros(9);  // digits 1..9, same "alphabet size" flavor
    auto cost_finite = min_generator_counts(finite, x_max);
    Length f_finite = expansion_from_cost_table(cost_finite, s);

    INFO("f_log(15)=" << f_log << " f_finite(15)=" << f_finite);
    REQUIRE(f_log > f_finite);
    // The gap should be large (place notation should be at least an order
    // of magnitude ahead at this budget).
    REQUIRE(f_log > 10 * f_finite);
}
