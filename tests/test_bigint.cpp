#include <catch2/catch_test_macros.hpp>
#include <cmath>

#include "utils/bigint.hpp"

using namespace hmcompress::utils;

TEST_CASE("BigInt: construction and to_string", "[bigint]") {
    REQUIRE(BigInt(0).to_string() == "0");
    REQUIRE(BigInt(42).to_string() == "42");
    REQUIRE(BigInt(1'000'000'000ULL).to_string() == "1000000000");
    REQUIRE(BigInt("123456789012345678901234567890").to_string() ==
            "123456789012345678901234567890");
}

TEST_CASE("BigInt: addition", "[bigint]") {
    BigInt a(999'999'999ULL);
    BigInt b(1ULL);
    a += b;
    REQUIRE(a.to_string() == "1000000000");
}

TEST_CASE("BigInt: multiplication by scalar", "[bigint]") {
    BigInt a(100);
    a *= 1'000'000'000ULL;
    REQUIRE(a.to_string() == "100000000000");
}

TEST_CASE("BigInt: multiplication of two BigInts", "[bigint]") {
    BigInt a("999999999999");
    BigInt b("999999999999");
    BigInt c = a * b;
    // 999999999999^2 = 999999999998000000000001
    REQUIRE(c.to_string() == "999999999998000000000001");
}

TEST_CASE("BigInt: Pow reproduces the paper's Googol-scale example", "[bigint]") {
    // 10^100 = Googol, referenced in the paper's introduction (footnote 2).
    BigInt googol = BigInt::Pow(10, 100);
    std::string s = googol.to_string();
    REQUIRE(s.size() == 101);  // "1" followed by 100 zeros
    REQUIRE(s[0] == '1');
    for (std::size_t i = 1; i < s.size(); ++i) REQUIRE(s[i] == '0');
}

TEST_CASE("BigInt: log10 is accurate for powers of 10", "[bigint]") {
    for (std::uint64_t exp : {1, 5, 10, 50, 104}) {
        BigInt val = BigInt::Pow(10, exp);
        REQUIRE(std::abs(val.log10() - static_cast<double>(exp)) < 1e-6);
    }
}

TEST_CASE("BigInt: log2 is consistent with log10", "[bigint]") {
    BigInt val = BigInt::Pow(2, 50);
    REQUIRE(std::abs(val.log2() - 50.0) < 1e-6);
}

TEST_CASE("BigInt: comparison operator", "[bigint]") {
    REQUIRE(BigInt(5) < BigInt(10));
    REQUIRE_FALSE(BigInt(10) < BigInt(5));
    REQUIRE_FALSE(BigInt(7) < BigInt(7));
    REQUIRE(BigInt("999999999999") < BigInt("1000000000000"));
}
