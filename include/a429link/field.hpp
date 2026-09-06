// Copyright 2026 levelesw
// SPDX-License-Identifier: BSD-2-Clause

#ifndef A429LINK_INCLUDE_A429LINK_FIELD_HPP_
#define A429LINK_INCLUDE_A429LINK_FIELD_HPP_

#include <cstdint>
#include <expected>

#include "word.hpp"

namespace a429link {

template <std::uint32_t Lsb, std::uint32_t Width>
  requires(Lsb >= 1U && Lsb <= 32U && Width >= 1U && Width <= 33U - Lsb)
class Field final {
 public:
  static constexpr std::uint32_t kLsb = Lsb;
  static constexpr std::uint32_t kWidth = Width;

  Field() = delete;

  [[nodiscard]]
  static constexpr std::uint32_t Extract(Word word) noexcept {
    return word.Extract(Lsb, Width).value();
  }

  [[nodiscard]]
  static constexpr std::expected<Word, WordError> WithValue(
      Word word, std::uint32_t value) noexcept {
    return word.WithField(Lsb, Width, value);
  }
};

}  // namespace a429link

#endif  // A429LINK_INCLUDE_A429LINK_FIELD_HPP_
