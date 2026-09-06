// Copyright 2026 levelesw
// SPDX-License-Identifier: BSD-2-Clause

#include "a429link/a429/parity.hpp"

#include <gtest/gtest.h>

#include <cstdint>

#include "a429link/a429/standard_layout.hpp"

namespace {

using a429link::Word;
using a429link::a429::HasOddParity;
using Layout = a429link::a429::StandardLayout;
using a429link::a429::WithOddParity;

struct NoSdiLayout {
  using Label = a429link::Field<1U, 8U>;
  using Data = a429link::Field<9U, 21U>;
  using Ssm = a429link::Field<30U, 2U>;
  using Parity = a429link::Field<32U, 1U>;
};

struct OneBitSdiLayout {
  using Label = a429link::Field<1U, 8U>;
  using Sdi = a429link::Field<9U, 1U>;
  using Data = a429link::Field<10U, 20U>;
  using Ssm = a429link::Field<30U, 2U>;
  using Parity = a429link::Field<32U, 1U>;
};

struct NoSsmLayout {
  using Label = a429link::Field<1U, 8U>;
  using Sdi = a429link::Field<9U, 2U>;
  using Data = a429link::Field<11U, 21U>;
  using Parity = a429link::Field<32U, 1U>;
};

constexpr bool EverySingleBitFlipFailsParity() {
  constexpr auto valid = WithOddParity(Word::FromRaw(0x648D'168BU));
  for (std::uint32_t bit = 0U; bit < 32U; ++bit) {
    const auto flipped = Word::FromRaw(valid.raw() ^ (std::uint32_t{1} << bit));
    if (HasOddParity(flipped)) {
      return false;
    }
  }
  return true;
}

static_assert(noexcept(HasOddParity(Word{})));
static_assert(noexcept(WithOddParity(Word{})));
static_assert(!HasOddParity(Word{}));
static_assert(HasOddParity(Word::FromRaw(0x8000'0000U)));
static_assert(WithOddParity(Word{}).raw() == 0x8000'0000U);
static_assert(WithOddParity(Word::FromRaw(0xFFFF'FFFFU)).raw() == 0x7FFF'FFFFU);
static_assert(WithOddParity(WithOddParity(Word::FromRaw(0x1234'5678U))) ==
              WithOddParity(Word::FromRaw(0x1234'5678U)));
static_assert(EverySingleBitFlipFailsParity());

constexpr auto kLabeledWord = Layout::Label::WithValue(Word{}, 0213U);
static_assert(kLabeledWord.has_value());
constexpr auto kLabeledWordWithParity = WithOddParity(kLabeledWord.value());
static_assert(HasOddParity(kLabeledWordWithParity));
static_assert(Layout::Label::Extract(kLabeledWordWithParity) == 0213U);

void ExpectParityReplacement(std::uint32_t input, std::uint32_t expected) {
  const auto original = Word::FromRaw(input);
  const auto result = WithOddParity(original);

  EXPECT_EQ(result.raw(), expected);
  EXPECT_TRUE(HasOddParity(result));
  EXPECT_EQ(original.raw(), input);
}

// Purpose: Verify odd-parity validation over the complete word.
// Coverage: Check all-zero, all-one, bit 1, bit 32, and mixed patterns.
TEST(ParityTest, ValidatesAllThirtyTwoBits) {
  EXPECT_FALSE(HasOddParity(Word{}));
  EXPECT_FALSE(HasOddParity(Word::FromRaw(0xFFFF'FFFFU)));
  EXPECT_TRUE(HasOddParity(Word::FromRaw(0x0000'0001U)));
  EXPECT_TRUE(HasOddParity(Word::FromRaw(0x8000'0000U)));
  EXPECT_FALSE(HasOddParity(Word::FromRaw(0x8000'0001U)));
  EXPECT_FALSE(HasOddParity(Word::FromRaw(0x648D'168BU)));
  EXPECT_TRUE(HasOddParity(Word::FromRaw(0xE48D'168BU)));
}

// Purpose: Verify recalculation of bit 32 from bits 1 through 31.
// Coverage: Exercise all-zero, all-one, and both incoming parity-bit states.
TEST(ParityTest, RecalculatesParityIndependentOfItsPreviousValue) {
  ExpectParityReplacement(0x0000'0000U, 0x8000'0000U);
  ExpectParityReplacement(0xFFFF'FFFFU, 0x7FFF'FFFFU);
  ExpectParityReplacement(0x8000'0000U, 0x8000'0000U);
  ExpectParityReplacement(0x0000'0001U, 0x0000'0001U);
  ExpectParityReplacement(0x8000'0001U, 0x0000'0001U);
}

// Purpose: Verify preservation and idempotence when applying odd parity.
// Coverage: Preserve bits 1 through 31, leave the source unchanged, and apply
// parity twice to a mixed pattern.
TEST(ParityTest, PreservesDataBitsAndIsIdempotent) {
  const auto original = Word::FromRaw(0xDEAD'BEEFU);
  const auto once = WithOddParity(original);
  const auto twice = WithOddParity(once);

  EXPECT_EQ(once.raw() & 0x7FFF'FFFFU, original.raw() & 0x7FFF'FFFFU);
  EXPECT_EQ(twice, once);
  EXPECT_TRUE(HasOddParity(once));
  EXPECT_EQ(original.raw(), 0xDEAD'BEEFU);
}

// Purpose: Verify that odd parity detects corruption in every bit position.
// Coverage: Flip each of the 32 bits in a valid mixed-pattern word.
TEST(ParityTest, RejectsEverySingleBitFlip) {
  const auto valid = WithOddParity(Word::FromRaw(0x648D'168BU));

  for (std::uint32_t bit = 0U; bit < 32U; ++bit) {
    SCOPED_TRACE(::testing::Message() << "bit=" << bit + 1U);
    const auto flipped = Word::FromRaw(valid.raw() ^ (std::uint32_t{1} << bit));
    EXPECT_FALSE(HasOddParity(flipped));
  }
}

// Purpose: Verify standard-layout field composition with explicit parity.
// Coverage: Build the fixed Label, SDI, Data, and SSM example, compare raw
// values before and after parity, and read every field back.
TEST(ParityTest, BuildsStandardLayoutWordAndAppliesParity) {
  auto word = Word{};
  const auto with_label = Layout::Label::WithValue(word, 0213U);
  ASSERT_TRUE(with_label.has_value());
  word = with_label.value();
  const auto with_sdi = Layout::Sdi::WithValue(word, 2U);
  ASSERT_TRUE(with_sdi.has_value());
  word = with_sdi.value();
  const auto with_data = Layout::Data::WithValue(word, 0x12345U);
  ASSERT_TRUE(with_data.has_value());
  word = with_data.value();
  const auto with_ssm = Layout::Ssm::WithValue(word, 3U);
  ASSERT_TRUE(with_ssm.has_value());
  word = with_ssm.value();

  EXPECT_EQ(word.raw(), 0x648D'168BU);
  EXPECT_FALSE(HasOddParity(word));

  const auto with_parity = WithOddParity(word);
  EXPECT_EQ(with_parity.raw(), 0xE48D'168BU);
  EXPECT_TRUE(HasOddParity(with_parity));
  EXPECT_EQ(Layout::Label::Extract(with_parity), 0213U);
  EXPECT_EQ(Layout::Sdi::Extract(with_parity), 2U);
  EXPECT_EQ(Layout::Data::Extract(with_parity), 0x12345U);
  EXPECT_EQ(Layout::Ssm::Extract(with_parity), 3U);
  EXPECT_EQ(Layout::Parity::Extract(with_parity), 1U);
}

// Purpose: Verify parity support for consumer-defined field layouts.
// Coverage: Repurpose SDI bits as data, use a one-bit SDI, and repurpose SSM
// bits as data while preserving every configured value.
TEST(ParityTest, SupportsCustomLayoutsWithRepurposedBits) {
  const auto no_sdi_data = NoSdiLayout::Data::WithValue(Word{}, 3U);
  ASSERT_TRUE(no_sdi_data.has_value());
  const auto no_sdi_word = WithOddParity(no_sdi_data.value());
  EXPECT_EQ(NoSdiLayout::Data::Extract(no_sdi_word), 3U);
  EXPECT_TRUE(HasOddParity(no_sdi_word));

  const auto one_bit_sdi = OneBitSdiLayout::Sdi::WithValue(Word{}, 1U);
  ASSERT_TRUE(one_bit_sdi.has_value());
  const auto one_bit_data =
      OneBitSdiLayout::Data::WithValue(one_bit_sdi.value(), 1U);
  ASSERT_TRUE(one_bit_data.has_value());
  const auto one_bit_word = WithOddParity(one_bit_data.value());
  EXPECT_EQ(OneBitSdiLayout::Sdi::Extract(one_bit_word), 1U);
  EXPECT_EQ(OneBitSdiLayout::Data::Extract(one_bit_word), 1U);
  EXPECT_TRUE(HasOddParity(one_bit_word));

  const auto no_ssm_data = NoSsmLayout::Data::WithValue(Word{}, 0x180000U);
  ASSERT_TRUE(no_ssm_data.has_value());
  const auto no_ssm_word = WithOddParity(no_ssm_data.value());
  EXPECT_EQ(NoSsmLayout::Data::Extract(no_ssm_word), 0x180000U);
  EXPECT_TRUE(HasOddParity(no_ssm_word));
}

}  // namespace
