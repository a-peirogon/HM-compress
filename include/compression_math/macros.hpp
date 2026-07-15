#pragma once

#include "core.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <ranges>
#include <algorithm>
#include <format>

namespace compression_math {


template <typename Monoid>
struct Macro {
    std::string name;
    typename Monoid::Element expansion; // expansion in primitive generators
    std::uint64_t g_length;             // |expansion|_G

    Macro(std::string n, typename Monoid::Element e, std::uint64_t glen)
        : name(std::move(n)), expansion(std::move(e)), g_length(glen) {}
};


template <typename Monoid>
class MacroSet {
public:
    using Element = typename Monoid::Element;

    void add_macro(const std::string& name, const Element& expansion) {
        macros_.emplace_back(name, expansion, Monoid::length(expansion));
    }

    void add_macro(const std::string& name, const Element& expansion, std::uint64_t forced_length) {
        macros_.emplace_back(name, expansion, forced_length);
    }

    [[nodiscard]] const std::vector<Macro<Monoid>>& macros() const { return macros_; }

    [[nodiscard]] std::size_t count() const { return macros_.size(); }

    // Number of macros with G-length exactly ell
    [[nodiscard]] std::uint64_t count_at_length(std::uint64_t ell) const {
        return std::ranges::count_if(macros_, [ell](const auto& m) { return m.g_length == ell; });
    }

    // Number of macros with G-length <= r
    [[nodiscard]] std::uint64_t count_up_to_length(std::uint64_t r) const {
        return std::ranges::count_if(macros_, [r](const auto& m) { return m.g_length <= r; });
    }

    // Density: |M ∩ B_G(r)| / |B_G(r)|
    [[nodiscard]] double density(std::uint64_t r) const {
        if (r == 0) return 0.0;
        std::uint64_t macro_count = count_up_to_length(r);
        std::uint64_t ball = Monoid::ball_size(r);
        return static_cast<double>(macro_count) / static_cast<double>(ball);
    }

    // Sphere density for F_n: |M ∩ S_r| / |S_r|
    [[nodiscard]] double sphere_density(std::uint64_t r) const {
        if constexpr (requires { Monoid::sphere_size(r); }) {
            std::uint64_t macro_count = count_at_length(r);
            std::uint64_t sphere = Monoid::sphere_size(r);
            return static_cast<double>(macro_count) / static_cast<double>(sphere);
        } else {
            return 0.0;
        }
    }

    void clear() { macros_.clear(); }

private:
    std::vector<Macro<Monoid>> macros_;
};

// Predefined macro sets for A_n (Theorem 1, 3, 6, 7)
namespace factory {

template <std::size_t N>
MacroSet<FreeAbelianMonoid<N>> place_notation_macros(std::uint64_t base) {
    // M = { b^j * a_i : i=1..n, j>=1 }
    // We generate up to a practical limit for computation.
    MacroSet<FreeAbelianMonoid<N>> ms;
    for (std::size_t i = 0; i < N; ++i) {
        for (std::uint64_t j = 1; j < 64; ++j) {
            auto val = math::ipow(base, j);
            if (val == 0) break; // overflow guard
            auto gen = FreeAbelianMonoid<N>::generator(i);
            auto elem = FreeAbelianMonoid<N>::power(gen, val);
            ms.add_macro(std::format("g_{}_{}", i, j), elem);
        }
    }
    return ms;
}

template <std::size_t N>
MacroSet<FreeAbelianMonoid<N>> waring_macros(std::uint64_t k) {
    // M = { m^k * a_i : i=1..n, m>=1 }
    MacroSet<FreeAbelianMonoid<N>> ms;
    for (std::size_t i = 0; i < N; ++i) {
        for (std::uint64_t m = 1; m < 1000; ++m) {
            auto val = math::ipow(m, k);
            if (val == 0) break;
            auto gen = FreeAbelianMonoid<N>::generator(i);
            auto elem = FreeAbelianMonoid<N>::power(gen, val);
            ms.add_macro(std::format("pow{}_{}_{}", k, i, m), elem);
        }
    }
    return ms;
}

template <std::size_t N>
MacroSet<FreeAbelianMonoid<N>> double_log_macros(std::uint64_t base) {
    // M = { b^{b^j} * a_i : i=1..n, j>=0 }
    MacroSet<FreeAbelianMonoid<N>> ms;
    for (std::size_t i = 0; i < N; ++i) {
        for (std::uint64_t j = 0; j < 6; ++j) {
            auto inner = math::ipow(base, j);
            if (inner > 63) break; // avoid overflow in ipow
            auto val = math::ipow(base, inner);
            if (val == 0) break;
            auto gen = FreeAbelianMonoid<N>::generator(i);
            auto elem = FreeAbelianMonoid<N>::power(gen, val);
            ms.add_macro(std::format("tower_{}_{}", i, j), elem);
        }
    }
    return ms;
}

template <std::size_t N>
MacroSet<FreeAbelianMonoid<N>> finite_macros(const std::vector<std::uint64_t>& values) {
    // Finite macro set for A_1
    MacroSet<FreeAbelianMonoid<N>> ms;
    for (std::size_t i = 0; i < N; ++i) {
        for (auto v : values) {
            auto gen = FreeAbelianMonoid<N>::generator(i);
            auto elem = FreeAbelianMonoid<N>::power(gen, v);
            ms.add_macro(std::format("f_{}_{}", i, v), elem);
        }
    }
    return ms;
}

} // namespace factory

} // namespace compression_math
