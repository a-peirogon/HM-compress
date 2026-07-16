#pragma once
//
// utils/types.hpp
//
// Common type aliases used throughout the library. The paper's quantities
// (lengths, radii, densities) are naturally non-negative integers or
// reals, so fixed-width types are used throughout to avoid signed/unsigned
// mismatches in the DP tables.
//

#include <cstdint>
#include <limits>
#include <vector>

namespace hmcompress {

// A "length" in the monoid sense: |w|_G, |w|_{G'}, radius s, etc.
// 64-bit is sufficient for direct computation (unwrapped lengths in
// MathLib reach ~10^104 conceptually, but such large numbers are never
// materialized as plain integers -- see utils/bigint.hpp for that regime).
using Length = std::uint64_t;

// Sentinel for "unreachable" / "infinite cost" in DP tables.
inline constexpr Length kInfLength = std::numeric_limits<Length>::max() / 4;

// Index type for generators / macros within a generating set.
using GenIndex = std::size_t;

// A word in a free monoid F_n is a sequence of generator indices.
using Word = std::vector<GenIndex>;

// A vector in the free abelian monoid A_n: multiplicities per generator.
using AbelianElement = std::vector<std::uint64_t>;

}  // namespace hmcompress
