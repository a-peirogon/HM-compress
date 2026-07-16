#pragma once
//
// statistics/linear_regression.hpp
//
// Minimal ordinary least squares for pairs (x, y), used to check the
// paper's empirical claims of the form "log(unwrapped length) grows
// linearly with depth, slope close to 1 bit per level" (Section 3.6),
// or, on synthetic monoid data, to verify that log(f_{G'}(s)) grows
// linearly in s at slope log(b)/(n(b-1)) for Theorem 1's place notation,
// etc. (Table 1).
//
// Single-purpose OLS fit (y = slope * x + intercept) plus R^2, not a
// general statistics library.
//

#include <utility>
#include <vector>

namespace hmcompress::statistics {

struct LinearFit {
    double slope = 0.0;
    double intercept = 0.0;
    double r_squared = 0.0;
    std::size_t n_points = 0;
};

// Ordinary least squares fit of y ~ slope * x + intercept.
LinearFit fit_line(const std::vector<double>& xs, const std::vector<double>& ys);

// Convenience overload for (x, y) pairs.
LinearFit fit_line(const std::vector<std::pair<double, double>>& points);

}  // namespace hmcompress::statistics
