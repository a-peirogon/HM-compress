// examples/waring_expansion_demo.cpp
//
// Theorem 3 (Polynomial density gives infinite expansion). For any integer
// k >= 2, the macro set M = {m^k : m >= 1} has density |M ∩ B_G(r)| <=
// n*r^{1/k}, and f_{G'}(s) = infinity for all s >= n*g(k), where g(k) is
// the Waring constant.
//
// Checks, for k=2 (Lagrange's four-square theorem, g(2)=4), that every
// integer up to x_max can be represented as a sum of at most 4 squares --
// i.e. that cost[x] <= 4 for all x, confirming f_{G'}(4) = infinity
// (Remark 1).

#include <iomanip>
#include <iostream>

#include "expansion/coin_dp.hpp"
#include "macros/macro_set.hpp"

using namespace hmcompress;

int main() {
    const Length x_max = 100'000;
    const std::uint64_t k = 2;  // Lagrange's four-square theorem: g(2) = 4

    std::cout << "=== Theorem 3 / Remark 1: Waring macros, k=2 (squares) ===\n";
    std::cout << "Verifying every integer in [0, " << x_max
              << "] needs at most g(2)=4 square-macros.\n\n";

    auto macro_set = macros::waring_macros(k, x_max);
    auto cost = expansion::min_generator_counts(macro_set, x_max);

    Length max_cost_seen = 0;
    Length argmax = 0;
    for (Length x = 0; x <= x_max; ++x) {
        if (cost[x] > max_cost_seen) {
            max_cost_seen = cost[x];
            argmax = x;
        }
    }

    std::cout << "Maximum cost observed: " << max_cost_seen << " (at x = " << argmax << ")\n";
    std::cout << "Theorem 3 / Lagrange predicts: max cost <= 4\n";
    std::cout << (max_cost_seen <= 4 ? "PASS: f_G'(4) = infinity confirmed on this range.\n"
                                      : "UNEXPECTED: cost exceeded 4 somewhere.\n");

    // Cost distribution (expected concentration at 1-4).
    std::cout << "\nCost distribution:\n";
    std::vector<std::uint64_t> histogram(5, 0);
    for (Length x = 1; x <= x_max; ++x) {
        if (cost[x] <= 4) histogram[cost[x]]++;
    }
    for (int c = 1; c <= 4; ++c) {
        std::cout << "  cost=" << c << ": " << histogram[c] << " integers\n";
    }

    return 0;
}
