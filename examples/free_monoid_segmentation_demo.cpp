// examples/free_monoid_segmentation_demo.cpp
//
// Theorem 4 (Polynomial density gives linear expansion in F_n). Illustrates
// the contrast with A_n: even a reasonably rich macro dictionary over a
// free monoid alphabet only achieves LINEAR expansion, f_{G'}(s) = O(s),
// unlike the exponential expansion seen in A_n (Theorem 1).
//
// Polynomial-density macro dictionary over a binary alphabet {0,1} (all
// distinct substrings of length <= L, capped by a polynomial count).
// Measures |w|_{G'} for random strings, showing that expansion stays
// linear in the parse cost budget.

#include <iomanip>
#include <iostream>
#include <random>
#include <string>

#include "expansion/segmentation_dp.hpp"

using namespace hmcompress;
using namespace hmcompress::expansion;

namespace {

std::string random_binary_string(std::size_t length, std::mt19937& rng) {
    std::uniform_int_distribution<int> bit(0, 1);
    std::string s(length, '0');
    for (auto& c : s) c = static_cast<char>('0' + bit(rng));
    return s;
}

}  // namespace

int main() {
    std::mt19937 rng(42);

    // Polynomial-density dictionary: all binary strings of length 2..6
    // (a fixed, small alphabet-derived dictionary -- polynomial in the
    // sense that the count of macros of length ell is bounded by 2^ell,
    // which for small ell is a modest constant, matching Theorem 4's
    // regime without requiring astronomically large dictionaries).
    WordMacroDictionary dict;
    for (std::size_t len = 2; len <= 6; ++len) {
        std::size_t count = static_cast<std::size_t>(1) << len;  // 2^len distinct words
        for (std::size_t i = 0; i < count; ++i) {
            std::string word(len, '0');
            for (std::size_t b = 0; b < len; ++b) {
                if ((i >> b) & 1) word[b] = '1';
            }
            dict.add(word);
        }
    }

    std::cout << "=== Theorem 4: polynomial-density macros in F_2 ===\n";
    std::cout << "Dictionary size: " << dict.size() << " macros (lengths 2..6)\n\n";

    std::cout << std::setw(12) << "word_length" << std::setw(20) << "|w|_G'"
              << std::setw(24) << "ratio (|w|_G / |w|_G')\n";

    for (std::size_t len : {50, 100, 200, 400, 800, 1600}) {
        std::string w = random_binary_string(len, rng);
        Length cost = wrapped_length_single(w, dict);
        double ratio = static_cast<double>(len) / static_cast<double>(cost);
        std::cout << std::setw(12) << len << std::setw(20) << cost << std::setw(24)
                   << std::fixed << std::setprecision(3) << ratio << "\n";
    }

    std::cout << "\nExpect the ratio |w|_G / |w|_G' to stay roughly constant "
                 "(bounded), confirming LINEAR expansion (Theorem 4), in "
                 "contrast to A_n's exponential expansion under place notation.\n";
    return 0;
}
