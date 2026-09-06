// Copyright 2026 levelesw
// SPDX-License-Identifier: BSD-2-Clause

#include "a429link/a429/standard_layout.hpp"

#include <gtest/gtest.h>

#include <cstdint>

namespace {

using Layout = a429link::a429::StandardLayout;
using a429link::Word;
using a429link::WordError;

static_assert(Layout::Label::kLsb == 1U);
static_assert(Layout::Label::kWidth == 8U);
static_assert(Layout::Sdi::kLsb == 9U);
static_assert(Layout::Sdi::kWidth == 2U);
static_assert(Layout::Data::kLsb == 11U);
static_assert(Layout::Data::kWidth == 19U);
static_assert(Layout::Ssm::kLsb == 30U);
static_assert(Layout::Ssm::kWidth == 2U);
static_assert(Layout::Parity::kLsb == 32U);
static_assert(Layout::Parity::kWidth == 1U);

template <typename FieldType>
void ExpectAcceptsBoundariesAndRejectsOverflow(std::uint32_t maximum) {
  const auto minimum = FieldType::WithValue(Word{}, 0U);
  ASSERT_TRUE(minimum.has_value());
  EXPECT_EQ(FieldType::Extract(minimum.value()), 0U);

  const auto maximum_result = FieldType::WithValue(Word{}, maximum);
  ASSERT_TRUE(maximum_result.has_value());
  EXPECT_EQ(FieldType::Extract(maximum_result.value()), maximum);

  const auto overflow = FieldType::WithValue(Word{}, maximum + 1U);
  ASSERT_FALSE(overflow.has_value());
  EXPECT_EQ(overflow.error(), WordError::kValueOutOfRange);
}

// Purpose: Verify the standard A429 field positions and widths.
// Coverage: Check every named field through its public configuration constants.
TEST(StandardLayoutTest, DefinesStandardFieldRanges) {
  EXPECT_EQ(Layout::Label::kLsb, 1U);
  EXPECT_EQ(Layout::Label::kWidth, 8U);
  EXPECT_EQ(Layout::Sdi::kLsb, 9U);
  EXPECT_EQ(Layout::Sdi::kWidth, 2U);
  EXPECT_EQ(Layout::Data::kLsb, 11U);
  EXPECT_EQ(Layout::Data::kWidth, 19U);
  EXPECT_EQ(Layout::Ssm::kLsb, 30U);
  EXPECT_EQ(Layout::Ssm::kWidth, 2U);
  EXPECT_EQ(Layout::Parity::kLsb, 32U);
  EXPECT_EQ(Layout::Parity::kWidth, 1U);
}

// Purpose: Verify value boundaries for every standard field.
// Coverage: Accept zero and the maximum value and reject maximum plus one.
TEST(StandardLayoutTest, ValidatesEveryFieldValueRange) {
  ExpectAcceptsBoundariesAndRejectsOverflow<Layout::Label>(0xFFU);
  ExpectAcceptsBoundariesAndRejectsOverflow<Layout::Sdi>(0x3U);
  ExpectAcceptsBoundariesAndRejectsOverflow<Layout::Data>(0x7FFFFU);
  ExpectAcceptsBoundariesAndRejectsOverflow<Layout::Ssm>(0x3U);
  ExpectAcceptsBoundariesAndRejectsOverflow<Layout::Parity>(0x1U);
}

// Purpose: Verify the logical Label representation in the host integer.
// Coverage: Store octal Label 213 in bits 1 through 8 without bit reversal.
TEST(StandardLayoutTest, StoresLabelWithoutBitReversal) {
  const auto result = Layout::Label::WithValue(Word{}, 0213U);

  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value().raw(), 0x0000'008BU);
  EXPECT_EQ(Layout::Label::Extract(result.value()), 0213U);
}

// Purpose: Verify immutable field replacement without automatic parity updates.
// Coverage: Change Label, preserve every other bit including bit 32, and leave
// the source word unchanged.
TEST(StandardLayoutTest, ReplacesOnlyTheSelectedField) {
  const auto original = Word::FromRaw(0x8000'0000U);
  const auto result = Layout::Label::WithValue(original, 1U);

  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value().raw(), 0x8000'0001U);
  EXPECT_EQ(Layout::Parity::Extract(result.value()), 1U);
  EXPECT_EQ(original.raw(), 0x8000'0000U);
}

}  // namespace
