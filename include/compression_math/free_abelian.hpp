#pragma once

#include "core.hpp"
#include <array>
#include <numeric>
#include <algorithm>
#include <format>

namespace compression_math {

// Elements are vectors in N^n with componentwise addition.
template <std::size_t N>
class FreeAbelianMonoid {
public:
    using Element = std::array<std::uint64_t, N>;

    static constexpr std::size_t n_generators = N;

    [[nodiscard]] static std::uint64_t length(const Element& e) {
        return std::accumulate(e.begin(), e.end(), std::uint64_t{0});
    }

    [[nodiscard]] static Element identity() {
        return Element{}; // zero-initialized
    }

    [[nodiscard]] static Element generator(std::size_t i) {
        if (i >= N) throw ValidationError(std::format("generator index {} out of range [0,{})", i, N));
        Element e{};
        e[i] = 1;
        return e;
    }

    [[nodiscard]] static Element multiply(const Element& a, const Element& b) {
        Element res{};
        for (std::size_t i = 0; i < N; ++i) {
            res[i] = a[i] + b[i];
        }
        return res;
    }

    [[nodiscard]] static Element power(const Element& a, std::uint64_t k) {
        Element res{};
        for (std::size_t i = 0; i < N; ++i) {
            res[i] = a[i] * k;
        }
        return res;
    }

    [[nodiscard]] static bool equal(const Element& a, const Element& b) {
        return a == b;
    }

    [[nodiscard]] static std::uint64_t ball_size(std::uint64_t radius) {
        // |B_G(r)| = C(r + n, n)
        return math::binomial_coefficient(radius + N, N);
    }

    [[nodiscard]] static std::string to_string(const Element& e) {
        std::string s = "(";
        for (std::size_t i = 0; i < N; ++i) {
            s += std::to_string(e[i]);
            if (i + 1 < N) s += ", ";
        }
        s += ")";
        return s;
    }
};

} // namespace compression_math
