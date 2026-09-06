// Copyright 2026 levelesw
// SPDX-License-Identifier: BSD-2-Clause

#ifndef A429LINK_INCLUDE_A429LINK_A429_PARITY_HPP_
#define A429LINK_INCLUDE_A429LINK_A429_PARITY_HPP_

#include <bit>
#include <cstdint>

#include "a429link/word.hpp"

namespace a429link::a429 {

[[nodiscard]]
constexpr bool HasOddParity(Word word) noexcept {
  return (std::popcount(word.raw()) % 2) == 1;
}

[[nodiscard]]
constexpr Word WithOddParity(Word word) noexcept {
  constexpr auto kParityBitMask = std::uint32_t{1} << 31U;

  if (HasOddParity(word)) {
    return word;
  }

  // Flipping bit 32 changes the parity without modifying bits 1 through 31.
  return Word::FromRaw(word.raw() ^ kParityBitMask);
}

}  // namespace a429link::a429

#endif  // A429LINK_INCLUDE_A429LINK_A429_PARITY_HPP_
