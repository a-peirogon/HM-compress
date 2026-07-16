// examples/place_notation_demo.cpp
//
// Theorem 1 (Place notation gives exponential expansion). For A_n and any
// integer b >= 2, the macro set M = {b^j a_i} has logarithmic density and
// satisfies b^{s/(n(b-1))-1} <= f_{G'}(s) <= nb * b^{s/(n(b-1))}.
//
// Verifies the bounds numerically for A_1 (n=1) across several bases,
// printing measured f_{G'}(s) alongside the theoretical lower and upper
// bounds.

#include <cmath>
#include <iomanip>
#include <iostream>

#include "expansion/coin_dp.hpp"
#include "expansion/expansion_function.hpp"
#include "macros/macro_set.hpp"

using namespace hmcompress;

namespace {

// Theoretical bounds from Theorem 1, specialized to n=1:
//   b^{s-1} <= f_G'(s) <= b * b^s   (dividing the general formula's
//   exponent s/(n(b-1)) by (b-1) is already folded into these since for
//   n=1 the formula is b^{s/(b-1)}).
// Reproduces the exact paper formula rather than a further-simplified
// version, so the check is a faithful reproduction.
double lower_bound(std::uint64_t b, Length s) {
    return std::pow(static_cast<double>(b), static_cast<double>(s) / (b - 1) - 1.0);
}

double upper_bound(std::uint64_t b, Length s) {
    return static_cast<double>(b) * std::pow(static_cast<double>(b),
                                              static_cast<double>(s) / (b - 1));
}

}  // namespace

int main() {
    const Length x_max = 1'000'000;
    const Length s_max = 30;

    for (std::uint64_t base : {2u, 3u, 10u}) {
        std::cout << "\n=== Theorem 1, base b = " << base << " (A_1) ===\n";
        std::cout << std::setw(4) << "s" << std::setw(16) << "lower_bound"
                   << std::setw(20) << "measured f_G'(s)" << std::setw(18) << "upper_bound"
                   << "\n";

        auto macro_set = macros::place_notation_macros(base, x_max);
        auto cost = expansion::min_generator_counts(macro_set, x_max);
        auto f = expansion::expansion_curve(cost, s_max);

        for (Length s = 1; s <= s_max; ++s) {
            double lb = lower_bound(base, s);
            double ub = upper_bound(base, s);
            std::cout << std::setw(4) << s << std::setw(16) << std::scientific
                       << std::setprecision(3) << lb << std::setw(20) << std::fixed
                       << static_cast<double>(f[s]) << std::setw(18) << std::scientific
                       << ub << "\n";
        }
    }

    std::cout << "\nExpect measured f_G'(s) to lie between lower_bound and upper_bound "
                 "for all s (Theorem 1).\n";
    return 0;
}
