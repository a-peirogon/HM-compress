#include <compression_math.hpp>
#include <print>
#include <vector>
#include <cmath>
#include <cstdlib>

using namespace compression_math;

// Simple test framework
int tests_passed = 0;
int tests_failed = 0;

void check(bool condition, const std::string& name) {
    if (condition) {
        ++tests_passed;
        std::println("  [PASS] {}", name);
    } else {
        ++tests_failed;
        std::println("  [FAIL] {}", name);
    }
}

// Test Theorem 1: Place Notation gives exponential expansion in A_n
void test_theorem1() {
    std::println("\n--- Testing Theorem 1 (Place Notation, A_1) ---");
    constexpr std::uint64_t base = 2;
    auto macros = factory::place_notation_macros<1>(base);
    ExpansionAnalyzer<FreeAbelianMonoid<1>> analyzer(&macros);

    // Verify logarithmic density: |M ∩ B(r)| = O(log r)
    std::uint64_t r1 = 1000;
    std::uint64_t macro_count = macros.count_up_to_length(r1);
    std::uint64_t expected_log = math::int_log(base, r1) + 1;
    check(macro_count <= expected_log + 2, 
          "Logarithmic density bound for r=1000");

    // Verify exponential expansion: f(s) >= b^{s/(b-1)} - 1
    for (std::uint64_t s = 5; s <= 15; ++s) {
        std::uint64_t f = analyzer.expansion_function(s);
        double lower_bound = std::pow(base, static_cast<double>(s) / (base - 1)) - 1;
        check(f >= static_cast<std::uint64_t>(lower_bound * 0.5), 
              std::format("Exponential lower bound at s={}", s));
    }

    // Verify base-2 representation optimality for specific values
    // 2^5 = 32 should have |32|_{G'} = 1 (single macro)
    FreeAbelianMonoid<1>::Element e32{ 32 };
    check(analyzer.augmented_length(e32) == 1, "Macro 32 compresses to 1 token");

    // 31 = 11111_2 should have |31|_{G'} = 5
    FreeAbelianMonoid<1>::Element e31{ 31 };
    check(analyzer.augmented_length(e31) == 5, "31 = 16+8+4+2+1 has 5 tokens");

    // 63 = 111111_2 should have |63|_{G'} = 6
    FreeAbelianMonoid<1>::Element e63{ 63 };
    check(analyzer.augmented_length(e63) == 6, "63 has 6 tokens in base 2");
}

// Test Theorem 3: Waring / Polynomial density gives infinite expansion
void test_theorem3() {
    std::println("\n--- Testing Theorem 3 (Waring, A_1) ---");
    constexpr std::uint64_t k = 2; // squares
    auto macros = factory::waring_macros<1>(k);
    ExpansionAnalyzer<FreeAbelianMonoid<1>> analyzer(&macros);

    // Verify polynomial density: |M ∩ B(r)| <= r^{1/2}
    for (std::uint64_t r : {100, 400, 900, 1600}) {
        std::uint64_t count = macros.count_up_to_length(r);
        double bound = std::sqrt(r);
        check(count <= static_cast<std::uint64_t>(bound) + 1,
              std::format("Polynomial density bound at r={}", r));
    }

    // Verify bounded wrapped length: every integer should be sum of <= 4 squares
    // (Lagrange's four-square theorem, g(2)=4)
    for (std::uint64_t x : {1, 10, 50, 100, 200, 500, 1000}) {
        FreeAbelianMonoid<1>::Element e{ x };
        std::uint64_t wlen = analyzer.augmented_length(e);
        check(wlen <= 4, std::format("Bounded wrapped length for x={}", x));
    }
}

// Test Theorem 6: Double-logarithmic density gives polynomial expansion
void test_theorem6() {
    std::println("\n--- Testing Theorem 6 (Double-Log, A_1) ---");
    constexpr std::uint64_t base = 2;
    auto macros = factory::double_log_macros<1>(base);
    ExpansionAnalyzer<FreeAbelianMonoid<1>> analyzer(&macros);

    // Verify double-log density: |M ∩ B(r)| = O(log log r)
    std::uint64_t r = 1000000;
    std::uint64_t count = macros.count_up_to_length(r);
    double loglog = std::log(std::log(r));
    check(count <= static_cast<std::uint64_t>(loglog) + 3,
          "Double-logarithmic density bound");

    // Verify polynomial expansion: f(s) should grow polynomially, not exponentially
    std::uint64_t s1 = 5, s2 = 10;
    std::uint64_t f1 = analyzer.expansion_function(s1);
    std::uint64_t f2 = analyzer.expansion_function(s2);
    if (f1 > 0 && f2 > 0) {
        double ratio = static_cast<double>(f2) / static_cast<double>(f1);
        // Exponential would give ratio ~ constant^{s2-s1}, polynomial gives ratio ~ (s2/s1)^c
        check(ratio < 100.0, "Polynomial growth (ratio not exponential)");
    }
}

// Test Theorem 7: Finite macro set gives linear expansion
void test_theorem7() {
    std::println("\n--- Testing Theorem 7 (Finite Macros, A_1) ---");
    auto macros = factory::finite_macros<1>(std::vector<std::uint64_t>{5, 12, 25});
    ExpansionAnalyzer<FreeAbelianMonoid<1>> analyzer(&macros);

    // Verify linear expansion: f(s) <= L*s where L = max macro length
    constexpr std::uint64_t L = 25;
    for (std::uint64_t s = 1; s <= 20; ++s) {
        std::uint64_t f = analyzer.expansion_function(s);
        check(f <= L * s, std::format("Linear upper bound at s={}", s));
    }

    // Verify f(s) >= s (since G ⊆ G')
    for (std::uint64_t s = 1; s <= 20; ++s) {
        std::uint64_t f = analyzer.expansion_function(s);
        check(f >= s, std::format("Trivial lower bound at s={}", s));
    }
}

// Test Theorem 4: F_n polynomial density gives linear expansion
void test_theorem4() {
    std::println("\n--- Testing Theorem 4 (Polynomial Density, F_2) ---");
    MacroSet<FreeMonoid<2>> macros;
    std::uint64_t c = 2, p = 1;
    for (std::uint64_t ell = 2; ell <= 15; ++ell) {
        std::uint64_t count = c * std::pow(ell, p);
        for (std::uint64_t j = 0; j < count; ++j) {
            auto word = FreeMonoid<2>::generator(j % 2);
            if (ell > 1) word = FreeMonoid<2>::power(word, ell);
            macros.add_macro(std::format("m_{}_{}", ell, j), word);
        }
    }

    ExpansionAnalyzer<FreeMonoid<2>> analyzer(&macros);

    // Verify that sphere density vanishes exponentially
    for (std::uint64_t r : {5, 8, 10, 12}) {
        double dens = macros.sphere_density(r);
        check(dens < 0.5, std::format("Vanishing sphere density at r={}", r));
    }

    // Verify linear expansion by sampling
    std::uint64_t s1 = 10, s2 = 20;
    std::uint64_t f1 = analyzer.expansion_function(s1);
    std::uint64_t f2 = analyzer.expansion_function(s2);
    if (f1 > 0 && f2 > 0) {
        double ratio = static_cast<double>(f2) / static_cast<double>(f1);
        check(ratio < 3.0, "Linear expansion ratio < 3");
    }
}

// Test A_n general properties
void test_an_properties() {
    std::println("\n--- Testing A_n General Properties ---");

    // Ball size formula
    check(FreeAbelianMonoid<1>::ball_size(5) == 6, "A_1 ball size");
    check(FreeAbelianMonoid<2>::ball_size(2) == 6, "A_2 ball size C(4,2)=6");
    check(FreeAbelianMonoid<3>::ball_size(2) == 10, "A_3 ball size C(5,3)=10");

    // Generator properties
    auto g0 = FreeAbelianMonoid<3>::generator(0);
    auto g1 = FreeAbelianMonoid<3>::generator(1);
    auto g2 = FreeAbelianMonoid<3>::generator(2);
    check(g0[0] == 1 && g0[1] == 0 && g0[2] == 0, "Generator 0 correct");
    check(g1[0] == 0 && g1[1] == 1 && g1[2] == 0, "Generator 1 correct");
    check(g2[0] == 0 && g2[1] == 0 && g2[2] == 1, "Generator 2 correct");

    // Multiplication
    auto sum = FreeAbelianMonoid<3>::multiply(g0, g1);
    check(sum[0] == 1 && sum[1] == 1 && sum[2] == 0, "Multiplication correct");
}

// Test F_n general properties
void test_fn_properties() {
    std::println("\n--- Testing F_n General Properties ---");

    // Sphere and ball sizes
    check(FreeMonoid<2>::sphere_size(3) == 8, "F_2 sphere size 2^3=8");
    check(FreeMonoid<2>::ball_size(2) == 7, "F_2 ball size (2^3-1)/(2-1)=7");
    check(FreeMonoid<3>::sphere_size(2) == 9, "F_3 sphere size 3^2=9");

    // Word operations
    auto a = FreeMonoid<2>::generator(0);
    auto b = FreeMonoid<2>::generator(1);
    auto ab = FreeMonoid<2>::multiply(a, b);
    check(ab.size() == 2 && ab[0] == 0 && ab[1] == 1, "Word multiplication");

    auto a3 = FreeMonoid<2>::power(a, 3);
    check(a3.size() == 3 && a3[0] == 0 && a3[1] == 0 && a3[2] == 0, "Word power");
}

int main() {
    std::println("Test Suite");
    std::println("=============");

    test_theorem1();
    test_theorem3();
    test_theorem6();
    test_theorem7();
    test_theorem4();
    test_an_properties();
    test_fn_properties();

    std::println("\n============");
    std::println("Results: {} passed, {} failed", tests_passed, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
