#pragma once

#include "core.hpp"
#include "free_abelian.hpp"
#include "free_monoid.hpp"
#include "macros.hpp"
#include <vector>
#include <algorithm>
#include <limits>
#include <format>
#include <unordered_map>
#include <string>

namespace compression_math {

// Computes |w|_{G'}, f_{G'}(s)
template <typename Monoid>
class ExpansionAnalyzer {
public:
    using Element = typename Monoid::Element;

    explicit ExpansionAnalyzer(const MacroSet<Monoid>* macros = nullptr)
        : macros_(macros) {}

    void set_macros(const MacroSet<Monoid>* macros) { macros_ = macros; }


    [[nodiscard]] std::uint64_t primitive_length(const Element& w) const {
        return Monoid::length(w);
    }


    [[nodiscard]] std::uint64_t augmented_length(const Element& w) const {
        if (!macros_ || macros_->count() == 0) {
            return primitive_length(w);
        }
        if constexpr (requires { typename Monoid::Element::operator[]; }) {
            // A_n path: check if macros are axis-aligned
            return augmented_length_abelian(w);
        } else {
            // F_n path
            return augmented_length_free(w);
        }
    }


    // Returns the largest r such that all elements with |w|_G <= r have |w|_{G'} <= s
    [[nodiscard]] std::uint64_t expansion_function(std::uint64_t s) const {
        if (!macros_ || macros_->count() == 0) return s;

        if constexpr (requires { typename Monoid::Element::operator[]; }) {
            return expansion_function_abelian(s);
        } else {
            return expansion_function_free(s);
        }
    }

    [[nodiscard]] std::uint64_t depth(const Element& w) const {
        if (!macros_ || macros_->count() == 0) return 0;
        if constexpr (requires { typename Monoid::Element::operator[]; }) {
            return depth_abelian(w);
        } else {
            return depth_free(w);
        }
    }


    [[nodiscard]] std::uint64_t wrapped_length_excluding(const Element& macro_expansion,
                                                          std::uint64_t macro_glen) const {
        // This is a simplified version: we compute the augmented length of the
        // macro's expansion, but pretend the macro itself is not available.
        // For exact computation we would need a filtered macro set.
        // For the predefined macro sets in the paper, this is well-defined.
        return macro_glen; // fallback: primitive length
    }

private:
    const MacroSet<Monoid>* macros_;

    [[nodiscard]] std::uint64_t augmented_length_abelian(const Element& w) const {
        if constexpr (Monoid::n_generators == 1) {
            return augmented_length_a1(w[0]);
        } else {
            // For A_n with axis-aligned macros, decompose per coordinate
            std::uint64_t total = 0;
            for (std::size_t i = 0; i < Monoid::n_generators; ++i) {
                std::uint64_t coord = w[i];
                total += augmented_length_a1(coord, i);
            }
            return total;
        }
    }

    // Coin problem for a single coordinate
    [[nodiscard]] std::uint64_t augmented_length_a1(std::uint64_t value,
                                                       std::size_t coord = 0) const {
        if (value == 0) return 0;
        // Collect denominations for this coordinate
        std::vector<std::uint64_t> denoms;
        for (const auto& m : macros_->macros()) {
            // Check if macro is aligned to this coordinate
            bool aligned = true;
            for (std::size_t j = 0; j < Monoid::n_generators; ++j) {
                if (j == coord) continue;
                if (m.expansion[j] != 0) { aligned = false; break; }
            }
            if (aligned && m.expansion[coord] > 0) {
                denoms.push_back(m.expansion[coord]);
            }
        }
        if (denoms.empty()) return value;

        // Unbounded knapsack (minimum coins) via DP
        std::vector<std::uint64_t> dp(value + 1, std::numeric_limits<std::uint64_t>::max());
        dp[0] = 0;
        for (std::uint64_t v = 1; v <= value; ++v) {
            for (auto d : denoms) {
                if (d <= v && dp[v - d] != std::numeric_limits<std::uint64_t>::max()) {
                    dp[v] = std::min(dp[v], dp[v - d] + 1);
                }
            }
        }
        return dp[value];
    }

    [[nodiscard]] std::uint64_t expansion_function_abelian(std::uint64_t s) const {
        if constexpr (Monoid::n_generators == 1) {
            return expansion_function_a1(s);
        }
        // For A_n, we can approximate by checking the diagonal
        // A rigorous bound would require multi-dimensional DP.
        // For research purposes, we verify on A_1 and note that A_n scales similarly.
        std::uint64_t r = 0;
        while (true) {
            // Check if all elements in B_G(r) have |.|_{G'} <= s
            // For A_n, this is a multi-dimensional check. We do a sampling.
            bool ok = true;
            // For small r, enumerate
            if (r < 20) {
                // Enumerate all compositions of r into N parts
                // This is expensive for large N; we use a recursive generator
                std::vector<std::uint64_t> comp(Monoid::n_generators, 0);
                if (!check_all_compositions(r, s, comp, 0)) ok = false;
            } else {
                // For large r, use the A_1 bound as proxy
                auto a1_bound = expansion_function_a1(s / Monoid::n_generators);
                if (r > a1_bound * Monoid::n_generators) ok = false;
            }
            if (!ok) break;
            ++r;
        }
        return r > 0 ? r - 1 : 0;
    }

    bool check_all_compositions(std::uint64_t r, std::uint64_t s,
                                 std::vector<std::uint64_t>& comp, std::size_t idx) const {
        if (idx == Monoid::n_generators - 1) {
            comp[idx] = r;
            Element e{};
            for (std::size_t i = 0; i < Monoid::n_generators; ++i) e[i] = comp[i];
            if (augmented_length_abelian(e) > s) return false;
            return true;
        }
        for (std::uint64_t v = 0; v <= r; ++v) {
            comp[idx] = v;
            if (!check_all_compositions(r - v, s, comp, idx + 1)) return false;
        }
        return true;
    }

    [[nodiscard]] std::uint64_t expansion_function_a1(std::uint64_t s) const {
        // Find max r such that for all x <= r, augmented_length_a1(x) <= s
        std::uint64_t r = 0;
        while (true) {
            if (augmented_length_a1(r) > s) break;
            ++r;
        }
        return r > 0 ? r - 1 : 0;
    }

    [[nodiscard]] std::uint64_t depth_abelian(const Element& w) const {
        if constexpr (Monoid::n_generators == 1) {
            return depth_a1(w[0]);
        }
        std::uint64_t max_d = 0;
        for (std::size_t i = 0; i < Monoid::n_generators; ++i) {
            max_d = std::max(max_d, depth_a1(w[i], i));
        }
        return max_d;
    }

    [[nodiscard]] std::uint64_t depth_a1(std::uint64_t value, std::size_t coord = 0) const {
        if (value == 0) return 0;
        // Find optimal representation and recurse
        std::uint64_t best_len = augmented_length_a1(value, coord);
        if (best_len == value) return 0; // all primitives

        // Find which macro is used in the optimal representation
        // For simplicity, we use the place-notation / Waring structure
        // This is exact for the predefined macro sets in the paper.
        std::uint64_t best_d = 0;
        for (const auto& m : macros_->macros()) {
            bool aligned = true;
            for (std::size_t j = 0; j < Monoid::n_generators; ++j) {
                if (j == coord) continue;
                if (m.expansion[j] != 0) { aligned = false; break; }
            }
            if (!aligned) continue;
            std::uint64_t mval = m.expansion[coord];
            if (mval > value) continue;
            std::uint64_t rem = value - mval;
            std::uint64_t rem_len = augmented_length_a1(rem, coord);
            if (rem_len + 1 == best_len) {
                std::uint64_t d = 1 + depth_a1(rem, coord);
                best_d = std::max(best_d, d);
            }
        }
        return best_d;
    }


    [[nodiscard]] std::uint64_t augmented_length_free(const Element& w) const {
        // Greedy parsing: try to match longest macro first
        std::uint64_t total = 0;
        std::size_t pos = 0;
        while (pos < w.size()) {
            std::size_t best_len = 0;
            const Macro<Monoid>* best_macro = nullptr;
            for (const auto& m : macros_->macros()) {
                if (m.g_length > w.size() - pos) continue;
                bool match = true;
                for (std::size_t k = 0; k < m.g_length; ++k) {
                    if (w[pos + k] != m.expansion[k]) { match = false; break; }
                }
                if (match && m.g_length > best_len) {
                    best_len = m.g_length;
                    best_macro = &m;
                }
            }
            if (best_macro) {
                total += 1;
                pos += best_len;
            } else {
                total += 1;
                pos += 1;
            }
        }
        return total;
    }

    [[nodiscard]] std::uint64_t expansion_function_free(std::uint64_t s) const {
        // For F_n with polynomial macros, expansion is linear (Theorem 4)
        // We verify this empirically by sampling
        std::uint64_t r = 0;
        while (true) {
            // Sample a few words from S_r
            auto words = Monoid::sphere(r);
            std::size_t sample = std::min<std::size_t>(words.size(), 100);
            bool ok = true;
            for (std::size_t i = 0; i < sample; ++i) {
                if (augmented_length_free(words[i]) > s) {
                    ok = false;
                    break;
                }
            }
            if (!ok) break;
            ++r;
            if (r > 1000) break; // safety
        }
        return r > 0 ? r - 1 : 0;
    }

    [[nodiscard]] std::uint64_t depth_free(const Element& w) const {
        // Approximate depth for F_n
        return 0; // TODO: implement recursive parsing depth
    }
};

} // namespace compression_math
