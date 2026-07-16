#include <catch2/catch_test_macros.hpp>
#include <cmath>

#include "monoid/free_monoid.hpp"

using namespace hmcompress::monoid;

TEST_CASE("FreeMonoid: length is string length", "[monoid][F_n]") {
    hmcompress::Word w = {0, 1, 0, 2, 1};
    REQUIRE(FreeMonoid::length(w) == 5);
    REQUIRE(FreeMonoid::length({}) == 0);
}

TEST_CASE("FreeMonoid: sphere_size is n^r", "[monoid][F_n]") {
    REQUIRE(FreeMonoid::sphere_size(0, 2) == 1.0L);
    REQUIRE(FreeMonoid::sphere_size(1, 2) == 2.0L);
    REQUIRE(FreeMonoid::sphere_size(10, 2) == 1024.0L);
    REQUIRE(FreeMonoid::sphere_size(3, 3) == 27.0L);
}

TEST_CASE("FreeMonoid: ball_size is geometric series (n^{r+1}-1)/(n-1)", "[monoid][F_n]") {
    // F_2: |B_G(r)| = 2^{r+1} - 1
    REQUIRE(FreeMonoid::ball_size(0, 2) == 1.0L);   // just the identity
    REQUIRE(FreeMonoid::ball_size(1, 2) == 3.0L);   // identity + 2 generators
    REQUIRE(FreeMonoid::ball_size(2, 2) == 7.0L);   // 1 + 2 + 4
    REQUIRE(FreeMonoid::ball_size(3, 2) == 15.0L);  // 1 + 2 + 4 + 8

    // F_1 degenerates to r+1 (matches A_1 since F_1 = A_1 = N)
    REQUIRE(FreeMonoid::ball_size(5, 1) == 6.0L);
}
