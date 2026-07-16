#pragma once
//
// utils/bigint.hpp
//
// A minimal arbitrary-precision non-negative integer, sufficient for the
// unwrapped lengths that appear in Section 2 (e.g. b^k for large k) and in
// the MathLib analysis (Section 3), where unwrapped length reaches ~10^104.
//
// This is not a general-purpose bignum library. It supports exactly the
// operations required by the paper's constructions:
//   - construction from a small integer / from a string of decimal digits
//   - addition, multiplication by a small integer, exponentiation (b^k)
//   - comparison
//   - log10() / log2() as doubles, for plotting on log scales
//   - to_string()
//
// Internally, base 10^9 limbs, little-endian.
//

#include <cstdint>
#include <string>
#include <vector>

namespace hmcompress::utils {

class BigInt {
public:
    BigInt() : limbs_{0} {}
    BigInt(std::uint64_t value);
    explicit BigInt(const std::string& decimal);

    static BigInt Zero() { return BigInt(0); }
    static BigInt One() { return BigInt(1); }

    // Returns base^exponent as a BigInt. exponent may be large; this runs in
    // O(exponent) multiplications by `base` which is adequate for the sizes
    // used in demos (exponent up to a few thousand). For larger exponents,
    // prefer log-domain arithmetic (see log10()).
    static BigInt Pow(std::uint64_t base, std::uint64_t exponent);

    BigInt& operator+=(const BigInt& other);
    BigInt& operator*=(std::uint64_t scalar);
    BigInt& operator*=(const BigInt& other);

    friend BigInt operator+(BigInt a, const BigInt& b) { a += b; return a; }
    friend BigInt operator*(BigInt a, std::uint64_t b) { a *= b; return a; }
    friend BigInt operator*(BigInt a, const BigInt& b) { a *= b; return a; }

    bool operator<(const BigInt& other) const;
    bool operator==(const BigInt& other) const { return limbs_ == other.limbs_; }
    bool operator!=(const BigInt& other) const { return !(*this == other); }

    // Approximate log10 / log2 as doubles. Safe even when the value has
    // hundreds of digits (does not overflow, does not require exact value).
    double log10() const;
    double log2() const;

    std::string to_string() const;

    bool is_zero() const { return limbs_.size() == 1 && limbs_[0] == 0; }

private:
    static constexpr std::uint64_t kBase = 1'000'000'000ULL;  // 10^9
    std::vector<std::uint32_t> limbs_;  // little-endian, base 10^9

    void trim();
};

}  // namespace hmcompress::utils
