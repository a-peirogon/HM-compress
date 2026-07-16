#include "macros/macro_set.hpp"

#include <algorithm>

namespace hmcompress::macros {

MacroSet::MacroSet(LengthGenerator length_at, Length max_length, std::uint64_t max_count)
    : length_at_(std::move(length_at)), max_length_(max_length), max_count_(max_count) {
    for (std::uint64_t idx = 0; idx < max_count_; ++idx) {
        Length len = length_at_(idx);
        if (len == 0 || len > max_length_) break;
        cached_lengths_.push_back(len);
    }
    std::sort(cached_lengths_.begin(), cached_lengths_.end());
}

std::vector<Length> MacroSet::lengths_up_to(Length r) const {
    std::vector<Length> result;
    for (auto len : cached_lengths_) {
        if (len <= r) result.push_back(len);
    }
    return result;
}

std::uint64_t MacroSet::count_up_to(Length r) const {
    return static_cast<std::uint64_t>(
        std::count_if(cached_lengths_.begin(), cached_lengths_.end(),
                       [r](Length len) { return len <= r; }));
}

MacroSet place_notation_macros(std::uint64_t base, Length max_length) {
    // g_j = b^j for j = 1, 2, 3, ... (index 0 -> j=1).
    return MacroSet(
        [base](std::uint64_t index) -> Length {
            Length val = 1;
            for (std::uint64_t i = 0; i < index + 1; ++i) val *= base;
            return val;
        },
        max_length);
}

MacroSet waring_macros(std::uint64_t k, Length max_length) {
    // g_m = m^k for m = 1, 2, 3, ... (index 0 -> m=1).
    return MacroSet(
        [k](std::uint64_t index) -> Length {
            std::uint64_t m = index + 1;
            Length val = 1;
            for (std::uint64_t i = 0; i < k; ++i) val *= m;
            return val;
        },
        max_length);
}

MacroSet double_logarithmic_macros(std::uint64_t base, Length max_length) {
    // g_j = b^(b^j) for j = 0, 1, 2, ...
    return MacroSet(
        [base](std::uint64_t index) -> Length {
            std::uint64_t exponent = 1;
            for (std::uint64_t i = 0; i < index; ++i) exponent *= base;
            Length val = 1;
            for (std::uint64_t i = 0; i < exponent; ++i) {
                val *= base;
                if (val == 0) return 0;  // overflow guard -> stop enumeration
            }
            return val;
        },
        max_length,
        /*max_count=*/64);  // double-log density: very few terms needed
}

MacroSet finite_macros(Length largest_length) {
    // {1, 2, ..., largest_length}: a finite macro set.
    return MacroSet(
        [](std::uint64_t index) -> Length { return index + 1; },
        largest_length, largest_length + 1);
}

}  // namespace hmcompress::macros
