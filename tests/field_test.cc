// Copyright 2026 levelesw
// SPDX-License-Identifier: BSD-2-Clause

#include "a429link/field.hpp"

#include <gtest/gtest.h>

#include <cstdint>

namespace {

template <std::uint32_t Lsb, std::uint32_t Width>
concept DefinesField = requires { typename a429link::Field<Lsb, Width>; };

template <typename FieldType>
constexpr bool ExtractsTo(a429link::Word word, std::uint32_t expected) {
  return FieldType::Extract(word) == expected;
}

template <typename FieldType>
constexpr bool WithValueYields(a429link::Word word, std::uint32_t value,
                               std::uint32_t expected) {
  const auto result = FieldType::WithValue(word, value);
  return result.has_value() && result.value().raw() == expected;
}

using LowByteField = a429link::Field<1U, 8U>;
using MiddleField = a429link::Field<6U, 6U>;
using HighBitField = a429link::Field<32U, 1U>;
using FullWordField = a429link::Field<1U, 32U>;

static_assert(DefinesField<1U, 1U>);
static_assert(DefinesField<1U, 32U>);
static_assert(DefinesField<32U, 1U>);
static_assert(!DefinesField<0U, 1U>);
static_assert(!DefinesField<33U, 1U>);
static_assert(!DefinesField<1U, 0U>);
static_assert(!DefinesField<1U, 33U>);
static_assert(!DefinesField<32U, 2U>);

static_assert(LowByteField::kLsb == 1U);
static_assert(LowByteField::kWidth == 8U);
static_assert(noexcept(LowByteField::Extract(a429link::Word{})));
static_assert(noexcept(LowByteField::WithValue(a429link::Word{},
                                               std::uint32_t{0})));
static_assert(ExtractsTo<MiddleField>(a429link::Word::FromRaw(0x0000'05A0U),
                                      0x2DU));
static_assert(WithValueYields<LowByteField>(
    a429link::Word::FromRaw(0xFFFF'FF00U), 0x5AU, 0xFFFF'FF5AU));

// Suite purpose: Verify the public contract of a compile-time configured Field.
// Suite contents: Configuration constants, extraction, immutable replacement,
// boundary fields, template constraints, and value validation.

// Purpose: Verify extraction using the field's compile-time bit range.
// Coverage: Exercise low, middle, high, and full-width fields while checking
// right alignment.
TEST(FieldTest, ExtractsConfiguredFields) {
  EXPECT_EQ(LowByteField::Extract(a429link::Word::FromRaw(0x1234'5678U)),
            0x78U);
  EXPECT_EQ(MiddleField::Extract(a429link::Word::FromRaw(0x0000'05A0U)), 0x2DU);
  EXPECT_EQ(HighBitField::Extract(a429link::Word::FromRaw(0x8000'0000U)), 1U);
  EXPECT_EQ(FullWordField::Extract(a429link::Word::FromRaw(0x1234'5678U)),
            0x1234'5678U);
}

// Purpose: Verify immutable replacement through a configured field.
// Coverage: Replace a middle field, preserve adjacent bits, and leave the
// source word unchanged.
TEST(FieldTest, ReplacesConfiguredFieldAndPreservesOtherBits) {
  using ByteField = a429link::Field<9U, 8U>;

  const auto original = a429link::Word::FromRaw(0xA5A5'F00FU);
  const auto updated = ByteField::WithValue(original, 0x12U);

  ASSERT_TRUE(updated.has_value());
  EXPECT_EQ(updated.value().raw(), 0xA5A5'120FU);
  EXPECT_EQ(original.raw(), 0xA5A5'F00FU);
}

// Purpose: Verify replacement at the compile-time range boundaries.
// Coverage: Set bit 32 and replace all 32 bits.
TEST(FieldTest, ReplacesBoundaryAndFullWidthFields) {
  const auto high_bit = HighBitField::WithValue(a429link::Word{}, 1U);
  ASSERT_TRUE(high_bit.has_value());
  EXPECT_EQ(high_bit.value().raw(), 0x8000'0000U);

  const auto full_word = FullWordField::WithValue(
      a429link::Word::FromRaw(0xFFFF'FFFFU), 0x1234'5678U);
  ASSERT_TRUE(full_word.has_value());
  EXPECT_EQ(full_word.value().raw(), 0x1234'5678U);
}

// Purpose: Verify validation of replacement values.
// Coverage: Reject the first value that does not fit an eight-bit field.
TEST(FieldTest, RejectsValueThatDoesNotFitConfiguredWidth) {
  const auto result = LowByteField::WithValue(a429link::Word{}, 0x100U);

  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), a429link::WordError::kValueOutOfRange);
}

}  // namespace
