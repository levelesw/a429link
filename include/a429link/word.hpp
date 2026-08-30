// Copyright 2026 levelesw
// SPDX-License-Identifier: BSD-2-Clause

#ifndef A429LINK_INCLUDE_A429LINK_WORD_HPP_
#define A429LINK_INCLUDE_A429LINK_WORD_HPP_

#include <cstdint>
#include <expected>

namespace a429link {

enum class WordError {
  kInvalidLsb,
  kInvalidWidth,
  kFieldExceedsWord,
  kValueOutOfRange,
};

// Immutable 32-bit raw value type without A429-specific semantics.
// Public bit positions are 1-origin.
class Word final {
 public:
  [[nodiscard]]
  static constexpr Word FromRaw(std::uint32_t value) noexcept;

  constexpr Word() noexcept = default;

  [[nodiscard]]
  constexpr std::uint32_t raw() const noexcept {
    return value_;
  }

  // Extracts `width` bits from `lsb` and returns them right-aligned.
  // Returns range errors in the order defined by the public contract.
  [[nodiscard]]
  constexpr std::expected<std::uint32_t, WordError> Extract(
      std::uint32_t lsb, std::uint32_t width) const noexcept;

  // Returns a new Word with only the target field replaced by `value`.
  // Leaves the original unchanged and reports invalid ranges or values.
  [[nodiscard]]
  constexpr std::expected<Word, WordError> WithField(
      std::uint32_t lsb, std::uint32_t width,
      std::uint32_t value) const noexcept;

  friend constexpr bool operator==(Word, Word) noexcept = default;

 private:
  // Validates `lsb` and `width` in contract order and returns a right-aligned
  // mask.
  [[nodiscard]]
  static constexpr std::expected<std::uint32_t, WordError> FieldMask(
      std::uint32_t lsb, std::uint32_t width) noexcept;

  std::uint32_t value_{0};
};

// Header-only implementation details follow.

constexpr Word Word::FromRaw(std::uint32_t value) noexcept {
  Word word;
  word.value_ = value;
  return word;
}

constexpr std::expected<std::uint32_t, WordError> Word::Extract(
    std::uint32_t lsb, std::uint32_t width) const noexcept {
  const auto mask = FieldMask(lsb, width);
  if (!mask.has_value()) {
    return std::unexpected(mask.error());
  }

  // A validated lsb makes the shift count safe and right-aligns the field.
  const auto shift = lsb - 1U;
  return (value_ >> shift) & mask.value();
}

constexpr std::expected<Word, WordError> Word::WithField(
    std::uint32_t lsb, std::uint32_t width,
    std::uint32_t value) const noexcept {
  const auto mask = FieldMask(lsb, width);
  if (!mask.has_value()) {
    return std::unexpected(mask.error());
  }

  if (value > mask.value()) {
    return std::unexpected(WordError::kValueOutOfRange);
  }

  const auto shift = lsb - 1U;
  const auto shifted_mask = mask.value() << shift;

  // Clear only the target field, preserving every bit outside it.
  return FromRaw((value_ & ~shifted_mask) | (value << shift));
}

constexpr std::expected<std::uint32_t, WordError> Word::FieldMask(
    std::uint32_t lsb, std::uint32_t width) noexcept {
  // This validation order is part of the public error contract.
  if (lsb < 1U || lsb > 32U) {
    return std::unexpected(WordError::kInvalidLsb);
  }
  if (width < 1U || width > 32U) {
    return std::unexpected(WordError::kInvalidWidth);
  }

  const auto zero_based_lsb = lsb - 1U;

  // The subtraction form avoids overflow in the equivalent
  // lsb + width - 1 check.
  if (width > 32U - zero_based_lsb) {
    return std::unexpected(WordError::kFieldExceedsWord);
  }

  // Form the full-width mask directly because shifting by 32 is undefined.
  if (width == 32U) {
    return ~std::uint32_t{0};
  }
  return (std::uint32_t{1} << width) - 1U;
}

}  // namespace a429link

#endif  // A429LINK_INCLUDE_A429LINK_WORD_HPP_
