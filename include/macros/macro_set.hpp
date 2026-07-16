#pragma once
//
// macros/macro_set.hpp
//
// A macro set M = {g_i}, each defined by g_i = w_i for some word w_i over
// the base generators G (Section 2: "additional generators, each defined
// by g_i = w_i"). The augmented generating set is G' = G u M.
//
// Generator-agnostic: stores, for each macro, its G-length (|g_i|_G, an
// integer or BigInt) and exposes density queries (macros per ball of
// radius r) as required by every theorem in Section 2.
//
// Concrete macro *families* (place notation, Waring/polynomial, double-log,
// finite, polynomial-density in F_n, probabilistic sparse in F_n) are
// provided as factory functions, one per theorem, mapping directly onto
// the paper's statements.
//

#include <cstdint>
#include <functional>
#include <vector>

#include "utils/types.hpp"

namespace hmcompress::macros {

// A single macro: its G-length (how much "value" it packs) is what all the
// density and expansion computations need. The actual defining word w_i
// is not needed for the monoid-side theorems -- only |g_i|_G.
struct Macro {
    Length g_length;  // |g_i|_G
};

// A macro set restricted to macros of G-length <= max_radius, generated
// lazily via a density-defining rule. Matches how each theorem in
// Section 2 specifies M: by a growth condition, not an explicit list.
class MacroSet {
public:
    // `length_at` enumerates the G-lengths of macros in index order
    // (e.g. for place notation base b: b, b^2, b^3, ...). Enumeration stops
    // once length_at(k) exceeds max_length or max_count is reached.
    using LengthGenerator = std::function<Length(std::uint64_t index)>;

    MacroSet(LengthGenerator length_at, Length max_length,
              std::uint64_t max_count = 1'000'000);

    // All macro lengths <= r (i.e. M ∩ B_G(r), represented by lengths only).
    std::vector<Length> lengths_up_to(Length r) const;

    // |M ∩ B_G(r)|, the density function each theorem bounds.
    std::uint64_t count_up_to(Length r) const;

    const std::vector<Length>& all_lengths() const { return cached_lengths_; }

private:
    LengthGenerator length_at_;
    Length max_length_;
    std::uint64_t max_count_;
    std::vector<Length> cached_lengths_;  // sorted ascending
};

// ---------------------------------------------------------------------
// Factory functions: one per macro family used in the paper's theorems.
// All return a MacroSet enumerating G-lengths only (sufficient for A_n
// expansion computations via coin_dp).
// ---------------------------------------------------------------------

// Theorem 1: place notation. M = {b^j : j >= 1}, logarithmic density.
MacroSet place_notation_macros(std::uint64_t base, Length max_length);

// Theorem 3 (specialized to A_1): M = {m^k : m >= 1}, polynomial density
// (growth exponent 1/k).
MacroSet waring_macros(std::uint64_t k, Length max_length);

// Theorem 6: double-logarithmic density. M = {b^(b^j) : j >= 0}.
MacroSet double_logarithmic_macros(std::uint64_t base, Length max_length);

// Theorem 7: a finite macro set {1, 2, ..., L} (the simplest example
// achieving Theta(s) expansion).
MacroSet finite_macros(Length largest_length);

}  // namespace hmcompress::macros
