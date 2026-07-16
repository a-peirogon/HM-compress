#pragma once
//
// expansion/segmentation_dp.hpp
//
// Computes |w|_{G'} for a word w in F_n, given a macro dictionary M (each
// macro being an explicit substring over G), via optimal parsing:
//
//   cost[0] = 0
//   cost[i] = 1 + min over macros/generators matching w[j..i) of cost[j]
//
// This is the F_n analogue of coin_dp.hpp -- same idea (minimum-cost
// covering DP), but over string positions instead of integer values, since
// F_n has no relations and order matters (Section 2.2).
//
// Matching is done with a simple hash-set-based dictionary lookup per
// substring length; for the macro families used in Theorem 4 (polynomial
// density) and Theorem 5 (probabilistic sparse), dictionaries are generated
// directly rather than reverse-engineered from an alphabet, so exact
// substring matching is what the theorems require.
//

#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "utils/types.hpp"

namespace hmcompress::expansion {

// A macro dictionary for F_n: explicit words (as strings over an alphabet
// of size n, symbols '0'..'9' or 'a'..'z' as convenient) that may be used
// as single generators in G'.
class WordMacroDictionary {
public:
    void add(const std::string& word) { words_by_length_[word.size()].insert(word); }

    bool contains(const std::string& substr) const {
        auto it = words_by_length_.find(substr.size());
        if (it == words_by_length_.end()) return false;
        return it->second.count(substr) > 0;
    }

    // Longest macro length present in the dictionary (used to bound the DP
    // inner loop window).
    std::size_t max_word_length() const;

    std::size_t size() const;

private:
    // length -> set of words of that length
    std::unordered_map<std::size_t, std::unordered_set<std::string>> words_by_length_;
};

// Returns cost[0..w.size()]: cost[i] = minimum number of generators
// (single alphabet symbols or dictionary macros) needed to parse the
// prefix w[0..i). cost[w.size()] = |w|_{G'}.
std::vector<Length> min_parse_cost(const std::string& w, const WordMacroDictionary& dict);

Length wrapped_length_single(const std::string& w, const WordMacroDictionary& dict);

}  // namespace hmcompress::expansion
