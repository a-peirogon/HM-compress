#include <catch2/catch_test_macros.hpp>
#include <cmath>

#include "statistics/linear_regression.hpp"

using namespace hmcompress::statistics;

TEST_CASE("fit_line: perfect line recovers exact slope and intercept", "[statistics]") {
    // y = 2x + 3, exactly.
    std::vector<double> xs = {0, 1, 2, 3, 4, 5};
    std::vector<double> ys = {3, 5, 7, 9, 11, 13};

    auto fit = fit_line(xs, ys);
    REQUIRE(std::abs(fit.slope - 2.0) < 1e-9);
    REQUIRE(std::abs(fit.intercept - 3.0) < 1e-9);
    REQUIRE(std::abs(fit.r_squared - 1.0) < 1e-9);
}

TEST_CASE("fit_line: constant y gives slope 0 and r_squared 1", "[statistics]") {
    std::vector<double> xs = {1, 2, 3, 4};
    std::vector<double> ys = {5, 5, 5, 5};
    auto fit = fit_line(xs, ys);
    REQUIRE(std::abs(fit.slope) < 1e-9);
    REQUIRE(std::abs(fit.intercept - 5.0) < 1e-9);
    REQUIRE(std::abs(fit.r_squared - 1.0) < 1e-9);
}

TEST_CASE("fit_line: throws on mismatched or empty input", "[statistics]") {
    REQUIRE_THROWS_AS(fit_line(std::vector<double>{1, 2}, std::vector<double>{1}),
                       std::invalid_argument);
    REQUIRE_THROWS_AS(fit_line(std::vector<double>{}, std::vector<double>{}),
                       std::invalid_argument);
}

TEST_CASE("fit_line: pair overload matches vector overload", "[statistics]") {
    std::vector<std::pair<double, double>> points = {{0, 1}, {1, 3}, {2, 5}};
    auto fit_pairs = fit_line(points);
    auto fit_vecs = fit_line(std::vector<double>{0, 1, 2}, std::vector<double>{1, 3, 5});
    REQUIRE(std::abs(fit_pairs.slope - fit_vecs.slope) < 1e-9);
    REQUIRE(std::abs(fit_pairs.intercept - fit_vecs.intercept) < 1e-9);
}

TEST_CASE("fit_line: recovers expected slope for Theorem 1-style log-linear data",
          "[statistics]") {
    // Simulate log_b(f_G'(s)) ~ s/(b-1) for b=3, i.e. slope 1/2.
    std::vector<double> xs, ys;
    for (int s = 1; s <= 20; ++s) {
        xs.push_back(static_cast<double>(s));
        ys.push_back(static_cast<double>(s) / 2.0);  // noiseless synthetic data
    }
    auto fit = fit_line(xs, ys);
    REQUIRE(std::abs(fit.slope - 0.5) < 1e-9);
}
