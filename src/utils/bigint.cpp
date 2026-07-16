#include "utils/bigint.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace hmcompress::utils {

BigInt::BigInt(std::uint64_t value) {
    if (value == 0) {
        limbs_ = {0};
        return;
    }
    while (value > 0) {
        limbs_.push_back(static_cast<std::uint32_t>(value % kBase));
        value /= kBase;
    }
}

BigInt::BigInt(const std::string& decimal) {
    if (decimal.empty()) {
        limbs_ = {0};
        return;
    }
    limbs_.clear();
    // Parse from the least-significant end, 9 digits at a time.
    int end = static_cast<int>(decimal.size());
    while (end > 0) {
        int start = std::max(0, end - 9);
        limbs_.push_back(
            static_cast<std::uint32_t>(std::stoul(decimal.substr(start, end - start))));
        end = start;
    }
    if (limbs_.empty()) limbs_.push_back(0);
    trim();
}

void BigInt::trim() {
    while (limbs_.size() > 1 && limbs_.back() == 0) limbs_.pop_back();
}

BigInt BigInt::Pow(std::uint64_t base, std::uint64_t exponent) {
    BigInt result = BigInt::One();
    BigInt b = BigInt(base);
    while (exponent > 0) {
        if (exponent & 1) result *= b;
        b *= b;
        exponent >>= 1;
    }
    return result;
}

BigInt& BigInt::operator+=(const BigInt& other) {
    std::size_t n = std::max(limbs_.size(), other.limbs_.size());
    limbs_.resize(n, 0);
    std::uint64_t carry = 0;
    for (std::size_t i = 0; i < n; ++i) {
        std::uint64_t sum = carry + limbs_[i];
        if (i < other.limbs_.size()) sum += other.limbs_[i];
        limbs_[i] = static_cast<std::uint32_t>(sum % kBase);
        carry = sum / kBase;
    }
    if (carry) limbs_.push_back(static_cast<std::uint32_t>(carry));
    trim();
    return *this;
}

BigInt& BigInt::operator*=(std::uint64_t scalar) {
    if (scalar == 0) {
        limbs_ = {0};
        return *this;
    }
    std::uint64_t carry = 0;
    for (auto& limb : limbs_) {
        __uint128_t prod = static_cast<__uint128_t>(limb) * scalar + carry;
        limb = static_cast<std::uint32_t>(prod % kBase);
        carry = static_cast<std::uint64_t>(prod / kBase);
    }
    while (carry) {
        limbs_.push_back(static_cast<std::uint32_t>(carry % kBase));
        carry /= kBase;
    }
    trim();
    return *this;
}

BigInt& BigInt::operator*=(const BigInt& other) {
    if (is_zero() || other.is_zero()) {
        limbs_ = {0};
        return *this;
    }
    std::vector<std::uint64_t> result(limbs_.size() + other.limbs_.size(), 0);
    for (std::size_t i = 0; i < limbs_.size(); ++i) {
        __uint128_t carry = 0;
        for (std::size_t j = 0; j < other.limbs_.size() || carry; ++j) {
            __uint128_t cur = result[i + j] + carry;
            if (j < other.limbs_.size()) {
                cur += static_cast<__uint128_t>(limbs_[i]) * other.limbs_[j];
            }
            result[i + j] = static_cast<std::uint64_t>(cur % kBase);
            carry = cur / kBase;
        }
    }
    limbs_.assign(result.begin(), result.end());
    // Downcast: values were kept < kBase, so this is safe.
    std::vector<std::uint32_t> narrowed(limbs_.size());
    for (std::size_t i = 0; i < limbs_.size(); ++i) {
        narrowed[i] = static_cast<std::uint32_t>(result[i]);
    }
    limbs_ = std::move(narrowed);
    trim();
    return *this;
}

bool BigInt::operator<(const BigInt& other) const {
    if (limbs_.size() != other.limbs_.size()) return limbs_.size() < other.limbs_.size();
    for (std::size_t i = limbs_.size(); i-- > 0;) {
        if (limbs_[i] != other.limbs_[i]) return limbs_[i] < other.limbs_[i];
    }
    return false;
}

double BigInt::log10() const {
    if (is_zero()) return -std::numeric_limits<double>::infinity();
    // Use the top few limbs for precision; each limb contributes exactly
    // 9 decimal digits except possibly the most significant one.
    double result = 0.0;
    std::size_t n = limbs_.size();
    // Take up to the top 3 limbs for a high-precision mantissa.
    double mantissa = 0.0;
    std::size_t limbs_used = std::min<std::size_t>(3, n);
    for (std::size_t i = 0; i < limbs_used; ++i) {
        mantissa = mantissa / static_cast<double>(kBase) + limbs_[n - 1 - i];
    }
    result = std::log10(mantissa) + 9.0 * static_cast<double>(n - 1);
    return result;
}

double BigInt::log2() const { return log10() / std::log10(2.0); }

std::string BigInt::to_string() const {
    std::string s = std::to_string(limbs_.back());
    for (std::size_t i = limbs_.size() - 1; i-- > 0;) {
        std::string chunk = std::to_string(limbs_[i]);
        s += std::string(9 - chunk.size(), '0') + chunk;
    }
    return s;
}

}  // namespace hmcompress::utils
