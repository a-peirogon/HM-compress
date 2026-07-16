#include <catch2/catch_test_macros.hpp>

#include "expansion/segmentation_dp.hpp"

using namespace hmcompress::expansion;

TEST_CASE("WordMacroDictionary: contains and max_word_length", "[segmentation_dp]") {
    WordMacroDictionary dict;
    REQUIRE(dict.max_word_length() == 0);
    REQUIRE(dict.size() == 0);

    dict.add("ab");
    dict.add("abc");
    dict.add("xy");

    REQUIRE(dict.contains("ab"));
    REQUIRE(dict.contains("abc"));
    REQUIRE_FALSE(dict.contains("ac"));
    REQUIRE(dict.max_word_length() == 3);
    REQUIRE(dict.size() == 3);
}

TEST_CASE("min_parse_cost: empty dictionary gives cost = word length", "[segmentation_dp]") {
    WordMacroDictionary dict;  // no macros
    std::string w = "aaaaa";
    auto cost = min_parse_cost(w, dict);
    REQUIRE(cost.back() == w.size());
}

TEST_CASE("min_parse_cost: a single reusable macro compresses repeated patterns",
          "[segmentation_dp]") {
    WordMacroDictionary dict;
    dict.add("ab");  // one macro of length 2

    // "ababab" = "ab"+"ab"+"ab": cost should be 3 (three macro uses),
    // versus 6 without the macro.
    std::string w = "ababab";
    auto cost = min_parse_cost(w, dict);
    REQUIRE(cost.back() == 3);
}

TEST_CASE("min_parse_cost: matches expectation on a hand-built example", "[segmentation_dp]") {
    WordMacroDictionary dict;
    dict.add("aaa");  // length-3 macro

    // "aaaaaa" (length 6) = "aaa"+"aaa": cost 2.
    REQUIRE(wrapped_length_single("aaaaaa", dict) == 2);

    // "aaaaa" (length 5) = "aaa" + "a" + "a": cost 3 (no length-2 macro
    // available, so the remaining 2 chars cost 1 each).
    REQUIRE(wrapped_length_single("aaaaa", dict) == 3);
}

TEST_CASE("min_parse_cost: dictionary with overlapping-length macros picks the optimum",
          "[segmentation_dp]") {
    WordMacroDictionary dict;
    dict.add("aa");    // length 2
    dict.add("aaaa");  // length 4

    // "aaaaaaaa" (length 8): best is two uses of "aaaa" -> cost 2.
    REQUIRE(wrapped_length_single("aaaaaaaa", dict) == 2);
}
