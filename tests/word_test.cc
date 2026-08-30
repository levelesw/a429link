// Copyright 2026 levelesw
// SPDX-License-Identifier: BSD-2-Clause

#include "a429link/word.hpp"

#include <gtest/gtest.h>

#include <cstdint>

namespace {

constexpr bool ExtractsTo(a429link::Word word, std::uint32_t lsb,
                          std::uint32_t width, std::uint32_t expected) {
  const auto result = word.Extract(lsb, width);
  return result.has_value() && result.value() == expected;
}

constexpr bool ExtractErrorIs(a429link::Word word, std::uint32_t lsb,
                              std::uint32_t width,
                              a429link::WordError expected) {
  const auto result = word.Extract(lsb, width);
  return !result.has_value() && result.error() == expected;
}

constexpr bool WithFieldYields(a429link::Word word, std::uint32_t lsb,
                               std::uint32_t width, std::uint32_t value,
                               std::uint32_t expected) {
  const auto result = word.WithField(lsb, width, value);
  return result.has_value() && result.value().raw() == expected;
}

constexpr bool WithFieldErrorIs(a429link::Word word, std::uint32_t lsb,
                                std::uint32_t width, std::uint32_t value,
                                a429link::WordError expected) {
  const auto result = word.WithField(lsb, width, value);
  return !result.has_value() && result.error() == expected;
}

static_assert(a429link::Word{}.raw() == 0U);
static_assert(a429link::Word::FromRaw(0x1234'5678U).raw() == 0x1234'5678U);
static_assert(ExtractsTo(a429link::Word::FromRaw(0x0000'05A0U), 6U, 6U, 0x2DU));
static_assert(WithFieldYields(a429link::Word::FromRaw(0xFFFF'00FFU), 9U, 8U,
                              0x5AU, 0xFFFF'5AFFU));
static_assert(ExtractErrorIs(a429link::Word{}, 32U, 2U,
                             a429link::WordError::kFieldExceedsWord));
static_assert(WithFieldErrorIs(a429link::Word{}, 0U, 1U, 0U,
                               a429link::WordError::kInvalidLsb));
static_assert(WithFieldErrorIs(a429link::Word{}, 1U, 1U, 2U,
                               a429link::WordError::kValueOutOfRange));
static_assert(sizeof(a429link::Word) == sizeof(std::uint32_t));

void ExpectExtractsTo(a429link::Word word, std::uint32_t lsb,
                      std::uint32_t width, std::uint32_t expected) {
  SCOPED_TRACE(::testing::Message() << "raw=" << word.raw() << ", lsb=" << lsb
                                    << ", width=" << width);
  const auto result = word.Extract(lsb, width);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), expected);
}

void ExpectExtractError(a429link::Word word, std::uint32_t lsb,
                        std::uint32_t width, a429link::WordError expected) {
  SCOPED_TRACE(::testing::Message() << "raw=" << word.raw() << ", lsb=" << lsb
                                    << ", width=" << width);
  const auto result = word.Extract(lsb, width);
  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), expected);
}

void ExpectWithFieldYields(a429link::Word word, std::uint32_t lsb,
                           std::uint32_t width, std::uint32_t value,
                           std::uint32_t expected) {
  SCOPED_TRACE(::testing::Message()
               << "raw=" << word.raw() << ", lsb=" << lsb << ", width=" << width
               << ", value=" << value);
  const auto result = word.WithField(lsb, width, value);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value().raw(), expected);
}

void ExpectWithFieldError(a429link::Word word, std::uint32_t lsb,
                          std::uint32_t width, std::uint32_t value,
                          a429link::WordError expected) {
  SCOPED_TRACE(::testing::Message()
               << "raw=" << word.raw() << ", lsb=" << lsb << ", width=" << width
               << ", value=" << value);
  const auto result = word.WithField(lsb, width, value);
  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), expected);
}

// Suite purpose: Verify the public contract of the generic 32-bit Word type.
// Suite contents: Construction, comparison, extraction, immutable field
// replacement, range validation, error precedence, and value validation.

// Purpose: Verify the two supported construction paths.
// Content: Check the default zero value and preservation of an explicit raw
// value.
TEST(WordTest, ConstructsFromDefaultAndRawValues) {
  EXPECT_EQ(a429link::Word{}.raw(), 0U);
  EXPECT_EQ(a429link::Word::FromRaw(0x1234'5678U).raw(), 0x1234'5678U);
}

// Purpose: Verify that equality reflects the complete raw word value.
// Content: Compare equal raw values and two values that differ in their bits.
TEST(WordTest, ComparesByRawValue) {
  EXPECT_EQ(a429link::Word::FromRaw(0x1234'5678U),
            a429link::Word::FromRaw(0x1234'5678U));
  EXPECT_NE(a429link::Word::FromRaw(0x1234'5678U),
            a429link::Word::FromRaw(0x8765'4321U));
}

// Purpose: Verify field extraction across the full 32-bit word.
// Content: Exercise low, high, middle, and full-width fields while checking
// right alignment and isolation from adjacent bits.
TEST(WordTest, ExtractsAndRightAlignsFields) {
  ExpectExtractsTo(a429link::Word::FromRaw(0x0000'0001U), 1U, 1U, 1U);
  ExpectExtractsTo(a429link::Word::FromRaw(0x8000'0000U), 32U, 1U, 1U);
  ExpectExtractsTo(a429link::Word::FromRaw(0xABCD'1278U), 1U, 8U, 0x78U);
  ExpectExtractsTo(a429link::Word::FromRaw(0x0000'05A0U), 6U, 6U, 0x2DU);
  ExpectExtractsTo(a429link::Word::FromRaw(0x1234'5678U), 1U, 32U,
                   0x1234'5678U);

  // Fields in high-order bits are also returned right-aligned.
  ExpectExtractsTo(a429link::Word::FromRaw(0xA000'0000U), 29U, 4U, 0xAU);

  // Set bits immediately outside the field do not leak into the result.
  ExpectExtractsTo(a429link::Word::FromRaw(0xFFFF'00FFU), 9U, 8U, 0U);
}

// Purpose: Verify successful field replacement and bit preservation.
// Content: Set and clear boundary, middle, maximum-width, and maximum-value
// fields, and reject a value one past a field's capacity.
TEST(WordTest, ReplacesFieldsAndPreservesOtherBits) {
  ExpectWithFieldYields(a429link::Word{}, 1U, 1U, 1U, 0x0000'0001U);
  ExpectWithFieldYields(a429link::Word{}, 32U, 1U, 1U, 0x8000'0000U);
  ExpectWithFieldYields(a429link::Word{}, 9U, 8U, 0x5AU, 0x0000'5A00U);
  ExpectWithFieldYields(a429link::Word::FromRaw(0xFFFF'FFFFU), 9U, 8U, 0U,
                        0xFFFF'00FFU);
  ExpectWithFieldYields(a429link::Word::FromRaw(0xA5A5'F00FU), 9U, 8U, 0x12U,
                        0xA5A5'120FU);
  ExpectWithFieldYields(a429link::Word::FromRaw(0xFFFF'FFFFU), 1U, 32U,
                        0x1234'5678U, 0x1234'5678U);

  ExpectWithFieldYields(a429link::Word{}, 4U, 5U, 0x1FU, 0x0000'00F8U);
  ExpectWithFieldError(a429link::Word{}, 4U, 5U, 0x20U,
                       a429link::WordError::kValueOutOfRange);
  ExpectWithFieldYields(a429link::Word{}, 1U, 32U, 0xFFFF'FFFFU, 0xFFFF'FFFFU);
}

// Purpose: Verify that field replacement follows Word's immutable semantics.
// Content: Check the returned replacement and confirm that the source word is
// unchanged.
TEST(WordTest, LeavesOriginalUnchangedWhenReplacingField) {
  const auto original = a429link::Word::FromRaw(0xA5A5'F00FU);
  const auto updated = original.WithField(9U, 8U, 0x12U);

  ASSERT_TRUE(updated.has_value());
  EXPECT_EQ(updated.value().raw(), 0xA5A5'120FU);
  EXPECT_EQ(original.raw(), 0xA5A5'F00FU);
}

// Purpose: Verify rejection of every invalid field-range category.
// Content: Exercise zero, above-limit, oversized runtime, and end-overflowing
// inputs through both extraction and replacement APIs.
TEST(WordTest, RejectsInvalidFieldRanges) {
  using enum a429link::WordError;

  // Validate large runtime values at the API boundary without narrowing.
  const std::uint32_t oversized = 257U;
  ExpectExtractError(a429link::Word{}, oversized, 1U, kInvalidLsb);
  ExpectExtractError(a429link::Word{}, 1U, oversized, kInvalidWidth);
  ExpectWithFieldError(a429link::Word{}, oversized, 1U, 0U, kInvalidLsb);
  ExpectWithFieldError(a429link::Word{}, 1U, oversized, 0U, kInvalidWidth);

  ExpectExtractError(a429link::Word{}, 0U, 1U, kInvalidLsb);
  ExpectExtractError(a429link::Word{}, 33U, 1U, kInvalidLsb);
  ExpectExtractError(a429link::Word{}, 1U, 0U, kInvalidWidth);
  ExpectExtractError(a429link::Word{}, 1U, 33U, kInvalidWidth);
  ExpectExtractError(a429link::Word{}, 32U, 2U, kFieldExceedsWord);

  ExpectWithFieldError(a429link::Word{}, 0U, 1U, 0U, kInvalidLsb);
  ExpectWithFieldError(a429link::Word{}, 1U, 0U, 0U, kInvalidWidth);
  ExpectWithFieldError(a429link::Word{}, 32U, 2U, 0U, kFieldExceedsWord);
}

// Purpose: Verify the documented order of validation failures.
// Content: Combine invalid positions, widths, and values to ensure range errors
// take precedence according to the public contract.
TEST(WordTest, ReportsValidationErrorsInContractOrder) {
  using enum a429link::WordError;

  // kInvalidLsb takes precedence when both lsb and width are invalid.
  ExpectExtractError(a429link::Word{}, 0U, 0U, kInvalidLsb);
  ExpectExtractError(a429link::Word{}, 33U, 33U, kInvalidLsb);

  // Field range validation takes precedence over value validation.
  ExpectWithFieldError(a429link::Word{}, 0U, 0U, 0xFFFF'FFFFU, kInvalidLsb);
  ExpectWithFieldError(a429link::Word{}, 1U, 0U, 0xFFFF'FFFFU, kInvalidWidth);
  ExpectWithFieldError(a429link::Word{}, 32U, 2U, 0xFFFF'FFFFU,
                       kFieldExceedsWord);
}

// Purpose: Verify that replacement values must fit the requested field width.
// Content: Try the first unrepresentable values for one-bit and eight-bit
// fields and check for kValueOutOfRange.
TEST(WordTest, RejectsValuesThatDoNotFitTheField) {
  ExpectWithFieldError(a429link::Word{}, 1U, 1U, 2U,
                       a429link::WordError::kValueOutOfRange);
  ExpectWithFieldError(a429link::Word{}, 1U, 8U, 0x100U,
                       a429link::WordError::kValueOutOfRange);
}

}  // namespace
