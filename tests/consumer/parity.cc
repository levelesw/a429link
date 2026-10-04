// Copyright 2026 levelesw
// SPDX-License-Identifier: BSD-2-Clause

#include "a429link/a429/parity.hpp"

a429link::Word ApplyParity(a429link::Word word)
{
    return a429link::a429::WithOddParity(word);
}