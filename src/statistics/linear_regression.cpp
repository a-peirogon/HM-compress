#include "statistics/linear_regression.hpp"

#include <cmath>
#include <stdexcept>

namespace hmcompress::statistics {

LinearFit fit_line(const std::vector<double>& xs, const std::vector<double>& ys) {
    if (xs.size() != ys.size() || xs.empty()) {
        throw std::invalid_argument("fit_line: xs and ys must be non-empty and equal length");
    }
    const std::size_t n = xs.size();
    double sum_x = 0.0, sum_y = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        sum_x += xs[i];
        sum_y += ys[i];
    }
    double mean_x = sum_x / static_cast<double>(n);
    double mean_y = sum_y / static_cast<double>(n);

    double s_xy = 0.0, s_xx = 0.0, s_yy = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        double dx = xs[i] - mean_x;
        double dy = ys[i] - mean_y;
        s_xy += dx * dy;
        s_xx += dx * dx;
        s_yy += dy * dy;
    }

    LinearFit result;
    result.n_points = n;
    if (s_xx == 0.0) {
        // Degenerate: all x identical. Slope undefined; report 0 and flag
        // via r_squared = 0.
        result.slope = 0.0;
        result.intercept = mean_y;
        result.r_squared = 0.0;
        return result;
    }
    result.slope = s_xy / s_xx;
    result.intercept = mean_y - result.slope * mean_x;
    if (s_yy > 0.0) {
        double ss_res = 0.0;
        for (std::size_t i = 0; i < n; ++i) {
            double predicted = result.slope * xs[i] + result.intercept;
            double residual = ys[i] - predicted;
            ss_res += residual * residual;
        }
        result.r_squared = 1.0 - ss_res / s_yy;
    } else {
        result.r_squared = 1.0;  // ys are constant and perfectly predicted
    }
    return result;
}

LinearFit fit_line(const std::vector<std::pair<double, double>>& points) {
    std::vector<double> xs, ys;
    xs.reserve(points.size());
    ys.reserve(points.size());
    for (const auto& [x, y] : points) {
        xs.push_back(x);
        ys.push_back(y);
    }
    return fit_line(xs, ys);
}

}  // namespace hmcompress::statistics
