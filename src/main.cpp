#include <compression_math.hpp>
#include <print>
#include <vector>
#include <cmath>

using namespace compression_math;

// ============================================================================
// Demo: Verification of Theorems from Section 2
// ============================================================================

void demo_theorem1_place_notation() {
    std::println("\n=== Theorem 1: Place Notation (Exponential Expansion) ===");
    constexpr std::uint64_t base = 10;
    auto macros = factory::place_notation_macros<1>(base);
    ExpansionAnalyzer<FreeAbelianMonoid<1>> analyzer(&macros);

    std::println("Macro set: M = {{ 10^j : j >= 1 }} (logarithmic density)");
    std::println("Density at r=1000: {:.6f}", macros.density(1000));
    std::println("Density at r=1000000: {:.6f}", macros.density(1000000));

    std::println("\nExpansion function f(s):");
    std::println("{:>4} {:>12} {:>12} {:>12}", "s", "f(s)", "base^{s/9}", "ratio");
    for (std::uint64_t s = 1; s <= 30; ++s) {
        std::uint64_t f = analyzer.expansion_function(s);
        double expected = std::pow(static_cast<double>(base), static_cast<double>(s) / 9.0);
        std::println("{:4} {:12} {:12.1f} {:12.3f}", s, f, expected, 
                     f > 0 ? static_cast<double>(f) / expected : 0.0);
    }

    // Verify exponential growth: f(s) ~ b^{s/(b-1)} for A_1, n=1
    std::println("\nVerifying f(s) grows exponentially...");
    std::uint64_t s1 = 20, s2 = 25;
    std::uint64_t f1 = analyzer.expansion_function(s1);
    std::uint64_t f2 = analyzer.expansion_function(s2);
    double ratio = static_cast<double>(f2) / static_cast<double>(f1);
    double expected_ratio = std::pow(base, (s2 - s1) / 9.0);
    std::println("f({})/f({}) = {:.3f}, expected ~ b^{}/9 = {:.3f}", 
                 s2, s1, ratio, s2 - s1, expected_ratio);
}

void demo_theorem3_waring() {
    std::println("\n=== Theorem 3: Waring / Polynomial Density (Infinite Expansion) ===");
    constexpr std::uint64_t k = 2; // squares
    auto macros = factory::waring_macros<1>(k);
    ExpansionAnalyzer<FreeAbelianMonoid<1>> analyzer(&macros);

    std::println("Macro set: M = {{ m^2 : m >= 1 }} (polynomial density, exponent 1/2)");
    std::println("Count at length <= 100: {}", macros.count_up_to_length(100));
    std::println("Count at length <= 10000: {}", macros.count_up_to_length(10000));

    // For Waring with k=2, g(2)=4 (Lagrange), so every integer is sum of <= 4 squares.
    // Thus f(s) = infinity for s >= 4.
    std::println("\nVerifying infinite expansion for s >= 4:");
    for (std::uint64_t s = 1; s <= 8; ++s) {
        std::uint64_t f = analyzer.expansion_function(s);
        std::println("f({}) = {}", s, f == std::numeric_limits<std::uint64_t>::max() ? 
                     "INF (or overflow)" : std::to_string(f));
    }

    // Demonstrate bounded wrapped length for large macros
    std::println("\nWrapped length of large macros (should be bounded by g(k)=4):");
    for (std::uint64_t m : {10, 50, 100, 500}) {
        FreeAbelianMonoid<1>::Element elem{ m * m };
        std::uint64_t wlen = analyzer.augmented_length(elem);
        std::println("m={}, m^2={}, |m^2|_{{G'}} = {}", m, m*m, wlen);
    }
}

void demo_theorem6_double_log() {
    std::println("\n=== Theorem 6: Double-Logarithmic Density (Polynomial Expansion) ===");
    constexpr std::uint64_t base = 2;
    auto macros = factory::double_log_macros<1>(base);
    ExpansionAnalyzer<FreeAbelianMonoid<1>> analyzer(&macros);

    std::println("Macro set: M = {{ 2^(2^j) : j >= 0 }} (double-log density)");
    std::println("Count at length <= 1000000: {}", macros.count_up_to_length(1000000));

    std::println("\nExpansion function f(s):");
    for (std::uint64_t s = 1; s <= 20; ++s) {
        std::uint64_t f = analyzer.expansion_function(s);
        std::println("f({}) = {}", s, f);
    }
    std::println("(Polynomial growth: f(s) ~ s^{{b/(b-1)}} = s^2)");
}

void demo_theorem7_finite() {
    std::println("\n=== Theorem 7: Finite Macro Set (Linear Expansion) ===");
    auto macros = factory::finite_macros<1>(std::vector<std::uint64_t>{5, 12, 25});
    ExpansionAnalyzer<FreeAbelianMonoid<1>> analyzer(&macros);

    std::println("Macro set: M = {{5, 12, 25}} (finite)");
    std::println("\nExpansion function f(s):");
    std::println("{:>4} {:>12} {:>12}", "s", "f(s)", "L*s (L=25)");
    for (std::uint64_t s = 1; s <= 20; ++s) {
        std::uint64_t f = analyzer.expansion_function(s);
        std::println("{:4} {:12} {:12}", s, f, 25 * s);
    }
    std::println("(Linear growth: f(s) = Theta(s))");
}

void demo_theorem4_fn_polynomial() {
    std::println("\n=== Theorem 4: F_n Polynomial Density (Linear Expansion) ===");
    // Build a polynomial-density macro set for F_2
    MacroSet<FreeMonoid<2>> macros;
    std::uint64_t c = 2, p = 1;
    for (std::uint64_t ell = 2; ell <= 20; ++ell) {
        std::uint64_t count = c * std::pow(ell, p);
        for (std::uint64_t j = 0; j < count && j < 100; ++j) {
            auto word = FreeMonoid<2>::generator(j % 2);
            if (ell > 1) word = FreeMonoid<2>::power(word, ell);
            macros.add_macro(std::format("m_{}_{}", ell, j), word);
        }
    }

    ExpansionAnalyzer<FreeMonoid<2>> analyzer(&macros);
    std::println("Macro set: polynomial density ~ {} * ell^{} for F_2", c, p);
    std::println("Total macros: {}", macros.count());

    std::println("\nExpansion function f(s) (sampled):");
    for (std::uint64_t s = 5; s <= 50; s += 5) {
        std::uint64_t f = analyzer.expansion_function(s);
        std::println("f({}) ~ {}", s, f);
    }
    std::println("(Linear growth expected: f(s) = O(s))");
}

void demo_metrics() {
    std::println("\n=== Metrics: Depth, Wrapped Length, Unwrapped Length ===");
    auto macros = factory::place_notation_macros<1>(10);
    ExpansionAnalyzer<FreeAbelianMonoid<1>> analyzer(&macros);

    std::println("{:>12} {:>12} {:>12} {:>12}", 
                 "Element", "|w|_G", "|w|_{G'}", "depth");
    for (std::uint64_t j = 1; j <= 6; ++j) {
        std::uint64_t val = math::ipow(10, j);
        FreeAbelianMonoid<1>::Element elem{ val };
        std::uint64_t prim = analyzer.primitive_length(elem);
        std::uint64_t aug = analyzer.augmented_length(elem);
        std::uint64_t d = analyzer.depth(elem);
        std::println("{:12} {:12} {:12} {:12}", val, prim, aug, d);
    }
}

int main() {
    std::println("Compression is All You Need: Modeling Mathematics");
    std::println("C++23 Research Library - Algebraic Model Implementation");
    std::println("======================================================");

    demo_theorem1_place_notation();
    demo_theorem3_waring();
    demo_theorem6_double_log();
    demo_theorem7_finite();
    demo_theorem4_fn_polynomial();
    demo_metrics();

    std::println("\n======================================================");
    std::println("All demos completed.");
    return 0;
}
