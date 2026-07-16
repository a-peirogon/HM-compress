#include "expansion/segmentation_dp.hpp"

#include <algorithm>

namespace hmcompress::expansion {

std::size_t WordMacroDictionary::max_word_length() const {
    std::size_t max_len = 0;
    for (const auto& [len, set] : words_by_length_) max_len = std::max(max_len, len);
    return max_len;
}

std::size_t WordMacroDictionary::size() const {
    std::size_t total = 0;
    for (const auto& [len, set] : words_by_length_) total += set.size();
    return total;
}

std::vector<Length> min_parse_cost(const std::string& w, const WordMacroDictionary& dict) {
    const std::size_t n = w.size();
    const std::size_t window = std::max<std::size_t>(1, dict.max_word_length());

    std::vector<Length> cost(n + 1, kInfLength);
    cost[0] = 0;
    for (std::size_t i = 1; i <= n; ++i) {
        // Single-symbol generator from G (always available, cost 1).
        if (cost[i - 1] != kInfLength) cost[i] = std::min(cost[i], cost[i - 1] + 1);
        // Try every macro ending at position i, starting within the window.
        std::size_t lo = (i > window) ? i - window : 0;
        for (std::size_t j = lo; j < i; ++j) {
            std::size_t len = i - j;
            if (len == 1) continue;  // already covered by the single-symbol case
            if (cost[j] == kInfLength) continue;
            std::string candidate = w.substr(j, len);
            if (dict.contains(candidate)) {
                cost[i] = std::min(cost[i], cost[j] + 1);
            }
        }
    }
    return cost;
}

Length wrapped_length_single(const std::string& w, const WordMacroDictionary& dict) {
    return min_parse_cost(w, dict).back();
}

}  // namespace hmcompress::expansion
