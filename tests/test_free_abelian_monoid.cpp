#include <catch2/catch_test_macros.hpp>

#include "monoid/free_abelian_monoid.hpp"

using namespace hmcompress::monoid;

TEST_CASE("FreeAbelianMonoid: generator and length", "[monoid][A_n]") {
    FreeAbelianMonoid a3(3);
    REQUIRE(a3.rank() == 3);

    auto g0 = a3.generator(0);
    REQUIRE(g0 == std::vector<std::uint64_t>{1, 0, 0});
    REQUIRE(FreeAbelianMonoid::length(g0) == 1);

    hmcompress::AbelianElement w = {2, 3, 5};
    REQUIRE(FreeAbelianMonoid::length(w) == 10);
}

TEST_CASE("FreeAbelianMonoid: ball_size matches stars-and-bars C(r+n, n)", "[monoid][A_n]") {
    // A_1: |B_G(r)| = r + 1 (integers 0..r)
    REQUIRE(FreeAbelianMonoid::ball_size(0, 1) == 1);
    REQUIRE(FreeAbelianMonoid::ball_size(5, 1) == 6);
    REQUIRE(FreeAbelianMonoid::ball_size(100, 1) == 101);

    // A_2: |B_G(r)| = C(r+2, 2) = (r+1)(r+2)/2
    REQUIRE(FreeAbelianMonoid::ball_size(0, 2) == 1);
    REQUIRE(FreeAbelianMonoid::ball_size(1, 2) == 3);  // (0,0),(1,0),(0,1)
    REQUIRE(FreeAbelianMonoid::ball_size(2, 2) == 6);
    REQUIRE(FreeAbelianMonoid::ball_size(10, 2) == 66);

    // A_3: |B_G(r)| = C(r+3, 3)
    REQUIRE(FreeAbelianMonoid::ball_size(1, 3) == 4);  // (0,0,0) + 3 unit vectors
    REQUIRE(FreeAbelianMonoid::ball_size(3, 3) == 20);
}
