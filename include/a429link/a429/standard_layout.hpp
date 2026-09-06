// Copyright 2026 levelesw
// SPDX-License-Identifier: BSD-2-Clause

#ifndef A429LINK_INCLUDE_A429LINK_A429_STANDARD_LAYOUT_HPP_
#define A429LINK_INCLUDE_A429LINK_A429_STANDARD_LAYOUT_HPP_

#include "a429link/field.hpp"

namespace a429link::a429 {

struct StandardLayout {
  using Label = Field<1U, 8U>;
  using Sdi = Field<9U, 2U>;
  using Data = Field<11U, 19U>;
  using Ssm = Field<30U, 2U>;
  using Parity = Field<32U, 1U>;
};

}  // namespace a429link::a429

#endif  // A429LINK_INCLUDE_A429LINK_A429_STANDARD_LAYOUT_HPP_
