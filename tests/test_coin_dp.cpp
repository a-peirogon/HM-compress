#include <catch2/catch_test_macros.hpp>

#include "expansion/coin_dp.hpp"
#include "macros/macro_set.hpp"

using namespace hmcompress;
using namespace hmcompress::expansion;

TEST_CASE("min_generator_counts: base generator only gives cost[x] = x", "[coin_dp]") {
    // No macros: {1} is the only denomination, so |x|_G' = x.
    auto empty_macros = macros::finite_macros(0);  // largest_length=0 -> empty set in practice
    // finite_macros(0) would produce lengths {} since index 0 -> 1 > 0 fails
    // immediately; verify emptiness assumption:
    REQUIRE(empty_macros.all_lengths().empty());

    auto cost = min_generator_counts(empty_macros, 20);
    for (Length x = 0; x <= 20; ++x) {
        REQUIRE(cost[x] == x);
    }
}

TEST_CASE("min_generator_counts: place notation base 10 matches decimal digit sum intuition",
          "[coin_dp]") {
    // With macros {10, 100, 1000, ...}, representing x optimally uses
    // (in the greedy/optimal sense) roughly the sum of decimal digits of x,
    // since each power of 10 can be used up to 9 times before the next
    // power becomes more efficient. A few small cases, verifiable by hand:
    auto m = macros::place_notation_macros(10, 10000);
    auto cost = min_generator_counts(m, 10000);

    // x=0: cost 0
    REQUIRE(cost[0] == 0);
    // x=1: one generator '1'
    REQUIRE(cost[1] == 1);
    // x=9: nine generators '1' (no macro helps, since smallest macro is 10)
    REQUIRE(cost[9] == 9);
    // x=10: one macro '10'
    REQUIRE(cost[10] == 1);
    // x=99: 9*10 + 9*1 = 9 tens + 9 ones = 18 generators
    REQUIRE(cost[99] == 18);
    // x=100: one macro '100'
    REQUIRE(cost[100] == 1);
    // x=999: 9 hundreds + 9 tens + 9 ones = 27
    REQUIRE(cost[999] == 27);
    // x=1000: one macro '1000'
    REQUIRE(cost[1000] == 1);
}

TEST_CASE("min_generator_counts: Waring k=2 respects Lagrange's four-square theorem",
          "[coin_dp]") {
    auto m = macros::waring_macros(2, 1000);
    auto cost = min_generator_counts(m, 1000);
    for (Length x = 0; x <= 1000; ++x) {
        REQUIRE(cost[x] <= 4);  // g(2) = 4, per Lagrange
    }
    // Some numbers genuinely require 4 squares (e.g. 7 = 4+1+1+1).
    REQUIRE(cost[7] == 4);
    // Perfect squares need only 1.
    REQUIRE(cost[16] == 1);
    REQUIRE(cost[100] == 1);
}

TEST_CASE("wrapped_length_single matches the last entry of the full table", "[coin_dp]") {
    auto m = macros::place_notation_macros(2, 1000);
    REQUIRE(wrapped_length_single(m, 500) == min_generator_counts(m, 500).back());
}
