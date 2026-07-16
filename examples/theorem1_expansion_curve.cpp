// examples/theorem1_expansion_curve.cpp
//
// Fits log_b(f_{G'}(s)) ~ slope * s + intercept for Theorem 1's place
// notation macros, and checks the measured slope against the paper's
// closed form: f_{G'}(s) = Theta(b^{s/(n(b-1))}), i.e. for n=1,
//   log_b(f_{G'}(s)) ~ s / (b - 1)
// so the expected slope is 1/(b-1).
//
// Mirrors the empirical methodology of Section 3.4 in the paper (fitting
// log2(unwrapped length) vs wrapped length to extract a slope in
// bits/token), applied here to the synthetic monoid model instead of
// MathLib data.

#include <cmath>
#include <iomanip>
#include <iostream>

#include "expansion/coin_dp.hpp"
#include "expansion/expansion_function.hpp"
#include "macros/macro_set.hpp"
#include "statistics/linear_regression.hpp"

using namespace hmcompress;

int main() {
    const Length x_max = 2'000'000;
    const Length s_max = 30;
    const Length s_min = 5;  // skip small-s transient behavior

    for (std::uint64_t base : {2u, 3u, 5u, 10u}) {
        auto macro_set = macros::place_notation_macros(base, x_max);
        auto cost = expansion::min_generator_counts(macro_set, x_max);
        auto f = expansion::expansion_curve(cost, s_max);

        std::vector<double> xs, ys;
        for (Length s = s_min; s <= s_max; ++s) {
            if (f[s] == 0) continue;
            xs.push_back(static_cast<double>(s));
            ys.push_back(std::log(static_cast<double>(f[s])) / std::log(static_cast<double>(base)));
        }

        auto fit = statistics::fit_line(xs, ys);
        double expected_slope = 1.0 / static_cast<double>(base - 1);

        std::cout << "base b=" << base << ":\n";
        std::cout << "  measured slope of log_b(f_G'(s)) vs s: " << std::fixed
                   << std::setprecision(4) << fit.slope << "  (R^2=" << fit.r_squared << ")\n";
        std::cout << "  theoretical slope (Theorem 1, n=1): 1/(b-1) = " << expected_slope
                   << "\n\n";
    }

    std::cout << "Expect measured slope to approach 1/(b-1) as s grows "
                 "(Theorem 1's asymptotic Theta bound).\n";
    return 0;
}
