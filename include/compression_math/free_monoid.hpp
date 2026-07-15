#pragma once

#include "core.hpp"
#include <vector>
#include <algorithm>
#include <format>

namespace compression_math {

// Elements are finite strings over an alphabet of n symbols.
template <std::size_t N>
class FreeMonoid {
public:
    using Symbol = std::size_t; // 0 .. N-1
    using Element = std::vector<Symbol>;

    static constexpr std::size_t n_generators = N;

    [[nodiscard]] static std::uint64_t length(const Element& e) {
        return e.size();
    }

    [[nodiscard]] static Element identity() {
        return Element{};
    }

    [[nodiscard]] static Element generator(std::size_t i) {
        if (i >= N) throw ValidationError(std::format("generator index {} out of range [0,{})", i, N));
        return Element{ i };
    }

    [[nodiscard]] static Element multiply(const Element& a, const Element& b) {
        Element res;
        res.reserve(a.size() + b.size());
        res.insert(res.end(), a.begin(), a.end());
        res.insert(res.end(), b.begin(), b.end());
        return res;
    }

    [[nodiscard]] static Element power(const Element& a, std::uint64_t k) {
        Element res;
        res.reserve(a.size() * k);
        for (std::uint64_t i = 0; i < k; ++i) {
            res.insert(res.end(), a.begin(), a.end());
        }
        return res;
    }

    [[nodiscard]] static bool equal(const Element& a, const Element& b) {
        return a == b;
    }

    [[nodiscard]] static std::uint64_t ball_size(std::uint64_t radius) {
        if (N == 1) return radius + 1;
        // (N^{r+1} - 1) / (N - 1)
        std::uint64_t num = math::ipow(N, radius + 1);
        if (num < math::ipow(N, radius + 1)) throw OverflowError("ball_size overflow");
        return (num - 1) / (N - 1);
    }

    [[nodiscard]] static std::uint64_t sphere_size(std::uint64_t radius) {
        return math::ipow(N, radius);
    }

    [[nodiscard]] static std::string to_string(const Element& e) {
        std::string s;
        for (auto sym : e) {
            s += static_cast<char>('a' + (sym % 26));
        }
        if (s.empty()) s = "ε";
        return s;
    }

    // Generate all words of exact length r
    [[nodiscard]] static std::vector<Element> sphere(std::uint64_t r) {
        std::vector<Element> result;
        if (r == 0) {
            result.push_back(identity());
            return result;
        }
        std::uint64_t total = sphere_size(r);
        result.reserve(total);
        Element current(r, 0);
        while (true) {
            result.push_back(current);
            // increment in base N
            std::size_t pos = r;
            while (pos > 0) {
                --pos;
                if (++current[pos] < N) break;
                current[pos] = 0;
            }
            if (pos == 0 && current[0] == 0) break;
        }
        return result;
    }
};

} // namespace compression_math
