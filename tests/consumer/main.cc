// Copyright 2026 levelesw
// SPDX-License-Identifier: BSD-2-Clause

#include "a429link/a429/standard_layout.hpp"

a429link::Word ApplyParity(a429link::Word word);

int main() {
  using Label = a429link::a429::StandardLayout::Label;

  const auto word = Label::WithValue(a429link::Word{}, 0213U);
  if (!word.has_value()) {
    return 1;
  }

  return ApplyParity(word.value()).raw() == 0x8000'008BU ? 0 : 1;
}