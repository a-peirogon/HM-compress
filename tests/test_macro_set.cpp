#include <catch2/catch_test_macros.hpp>

#include "macros/macro_set.hpp"

using namespace hmcompress::macros;

TEST_CASE("place_notation_macros: lengths are powers of the base", "[macros]") {
    auto m = place_notation_macros(/*base=*/10, /*max_length=*/1000);
    auto lengths = m.all_lengths();
    // Expect 10, 100, 1000 (10^4=10000 exceeds max_length)
    REQUIRE(lengths == std::vector<hmcompress::Length>{10, 100, 1000});
}

TEST_CASE("place_notation_macros: logarithmic density (Theorem 1)", "[macros]") {
    auto m = place_notation_macros(/*base=*/2, /*max_length=*/1'000'000);
    // Number of macros with length <= r is floor(log_b(r)), i.e. O(log r).
    REQUIRE(m.count_up_to(2) == 1);     // {2}
    REQUIRE(m.count_up_to(4) == 2);     // {2, 4}
    REQUIRE(m.count_up_to(1024) == 10); // {2,4,...,1024}
}

TEST_CASE("waring_macros: lengths are k-th powers", "[macros]") {
    auto m = waring_macros(/*k=*/2, /*max_length=*/100);
    auto lengths = m.all_lengths();
    // 1^2=1 (excluded, generator already has length 1... but factory
    // includes it), 2^2=4, ..., 10^2=100
    REQUIRE(lengths.front() == 1);
    REQUIRE(lengths.back() == 100);
    REQUIRE(lengths.size() == 10);  // 1..10 squared, all <= 100
}

TEST_CASE("waring_macros: polynomial density |M cap B_G(r)| <= n*r^{1/k}", "[macros]") {
    auto m = waring_macros(/*k=*/2, /*max_length=*/10000);
    // For k=2, count_up_to(r) should be floor(sqrt(r)).
    REQUIRE(m.count_up_to(100) == 10);
    REQUIRE(m.count_up_to(10000) == 100);
}

TEST_CASE("double_logarithmic_macros: lengths are b^(b^j)", "[macros]") {
    auto m = double_logarithmic_macros(/*base=*/2, /*max_length=*/1'000'000);
    // j=0: 2^1=2, j=1: 2^2=4, j=2: 2^4=16, j=3: 2^8=256, j=4: 2^16=65536
    auto lengths = m.all_lengths();
    REQUIRE(lengths == std::vector<hmcompress::Length>{2, 4, 16, 256, 65536});
}

TEST_CASE("finite_macros: exactly {1, ..., L}", "[macros]") {
    auto m = finite_macros(5);
    REQUIRE(m.all_lengths() == std::vector<hmcompress::Length>{1, 2, 3, 4, 5});
    REQUIRE(m.count_up_to(3) == 3);
    REQUIRE(m.count_up_to(100) == 5);  // capped at the finite set's size
}
