#pragma once

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include <format>
#include <cmath>
#include <ranges>
#include <algorithm>

namespace compression_math {

namespace math {

[[nodiscard]] constexpr std::uint64_t ipow(std::uint64_t base, std::uint64_t exp) {
    std::uint64_t result = 1;
    while (exp) {
        if (exp & 1) result *= base;
        base *= base;
        exp >>= 1;
    }
    return result;
}

[[nodiscard]] constexpr std::uint64_t int_log(std::uint64_t base, std::uint64_t value) {
    if (value == 0) return 0;
    std::uint64_t count = 0;
    while (value >= base) {
        value /= base;
        ++count;
    }
    return count;
}

[[nodiscard]] constexpr std::uint64_t int_log_ceil(std::uint64_t base, std::uint64_t value) {
    if (value == 0) return 0;
    std::uint64_t lg = int_log(base, value);
    return (math::ipow(base, lg) == value) ? lg : lg + 1;
}

[[nodiscard]] constexpr std::uint64_t binomial_coefficient(std::uint64_t n, std::uint64_t k) {
    if (k > n) return 0;
    if (k == 0 || k == n) return 1;
    k = std::min(k, n - k);
    std::uint64_t res = 1;
    for (std::uint64_t i = 0; i < k; ++i) {
        res = res * (n - i) / (i + 1);
    }
    return res;
}

} // namespace math

class OverflowError : public std::runtime_error {
public:
    explicit OverflowError(const std::string& msg) : std::runtime_error(msg) {}
};

class ValidationError : public std::runtime_error {
public:
    explicit ValidationError(const std::string& msg) : std::runtime_error(msg) {}
};


template <typename M>
concept Monoid = requires {
    typename M::Element;
    { M::n_generators } -> std::convertible_to<std::size_t>;
} && requires(typename M::Element a, typename M::Element b, std::size_t i) {
    { M::length(a) } -> std::convertible_to<std::uint64_t>;
    { M::identity() } -> std::same_as<typename M::Element>;
    { M::generator(i) } -> std::same_as<typename M::Element>;
    { M::multiply(a, b) } -> std::same_as<typename M::Element>;
    { M::equal(a, b) } -> std::convertible_to<bool>;
};

} // namespace compression_math
