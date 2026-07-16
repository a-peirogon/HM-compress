// src/main.cpp
//
// HM-compress -- v1 driver.
//
// Runs Theorem 1 (place notation) and Theorem 7 (finite macro set) side by
// side, illustrating exponential vs. linear expansion. See examples/ for
// theorem-by-theorem demos and tests/ for correctness checks against the
// closed-form bounds.

#include <iomanip>
#include <iostream>

#include "expansion/coin_dp.hpp"
#include "expansion/expansion_function.hpp"
#include "macros/macro_set.hpp"

using namespace hmcompress;

namespace {

void print_header(const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
}

}  // namespace

int main() {
    std::cout << "HM-compress -- v1 (algebraic model)\n";

    const Length x_max = 2000;
    const Length s_max = 40;

    print_header("Theorem 1: place notation (base b=10), exponential expansion");
    {
        auto macro_set = macros::place_notation_macros(/*base=*/10, x_max);
        auto cost = expansion::min_generator_counts(macro_set, x_max);
        auto f = expansion::expansion_curve(cost, s_max);
        std::cout << "  s : f_G'(s)\n";
        for (Length s = 0; s <= s_max; s += 5) {
            std::cout << "  " << std::setw(2) << s << " : " << f[s] << "\n";
        }
        std::cout << "  (expect f_G'(s) ~ Theta(10^(s/9)) per Theorem 1 with n=1,b=10)\n";
    }

    print_header("Theorem 7: finite macro set {1,...,5}, linear expansion");
    {
        auto macro_set = macros::finite_macros(5);
        auto cost = expansion::min_generator_counts(macro_set, x_max);
        auto f = expansion::expansion_curve(cost, s_max);
        std::cout << "  s : f_G'(s)\n";
        for (Length s = 0; s <= s_max; s += 5) {
            std::cout << "  " << std::setw(2) << s << " : " << f[s] << "\n";
        }
        std::cout << "  (expect f_G'(s) = Theta(s) per Theorem 7)\n";
    }

    std::cout << "\nRun `examples/` binaries for theorem-specific deep dives, "
                 "and `ctest` to verify closed-form bounds.\n";
    return 0;
}
