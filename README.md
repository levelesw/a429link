# a429link

A transport-independent C++23 foundation for working with A429-style 32-bit
words.

## Requirements

- CMake 3.20 or newer
- A C++23 compiler and standard library with `std::expected` support

The library is header-only and has no external runtime dependencies. Tests use
GoogleTest; CMake uses an installed package when available and otherwise fetches
the pinned test dependency during configuration.

## Build and test

Configure a clean build directory, build the test executable, and run CTest:

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Consumers link the `a429link::a429link` interface target.

## Standard A429 example

```cpp
#include <a429link/a429/parity.hpp>
#include <a429link/a429/standard_layout.hpp>

using Layout = a429link::a429::StandardLayout;

constexpr auto with_label =
    Layout::Label::WithValue(a429link::Word{}, 0213U);
static_assert(with_label.has_value());

// Octal Label 213 is stored directly as 0x8B; this layer does not reverse it.
static_assert(with_label->raw() == 0x0000'008BU);

// Apply parity explicitly after all fields have been populated.
constexpr auto word = a429link::a429::WithOddParity(with_label.value());
static_assert(word.raw() == 0x8000'008BU);
static_assert(a429link::a429::HasOddParity(word));
static_assert(Layout::Label::Extract(word) == 0213U);
```

`StandardLayout` provides named `Field` aliases for the conventional raw word
layout:

| Field | Public bits | Width |
| --- | --- | ---: |
| `Label` | 1--8 | 8 |
| `Sdi` | 9--10 | 2 |
| `Data` | 11--29 | 19 |
| `Ssm` | 30--31 | 2 |
| `Parity` | 32 | 1 |

Public bit positions are 1-origin and map directly to host-integer bit positions
`n - 1`. Label `0213U` is the octal C++ literal whose value is `0x8BU`; `213U`
is decimal and represents a different value. Label bits are not reversed here,
and the integer representation does not define byte order or wire transmission
order.

Each field stores an unsigned raw value. `WithValue()` returns a new `Word` and
does not change the original or update parity automatically. Populate or replace
all fields first, then call `WithOddParity()` explicitly. `HasOddParity()` checks
all 32 bits without repairing invalid parity.

`StandardLayout` is an optional convenience preset rather than a validity rule
or a required layout interface. Consumers can compose `Field<Lsb, Width>` aliases
for layouts that omit or repurpose SDI and SSM bits. The parity helpers operate
on the complete `Word` independently of those field definitions.

`Field<Lsb, Width>` rejects invalid ranges through its template constraints.
`WithValue()` reports a value that does not fit the configured width with
`std::expected`. For ranges selected at runtime, `Word::Extract()` and
`Word::WithField()` accept an LSB and width directly and report invalid ranges in
the same way.

## Current scope

The library currently provides the immutable `a429link::Word` value type,
validated runtime and compile-time fields, the optional A429 standard-layout
preset, and helpers for generating and validating odd parity. Bit positions are
1-origin, and `raw()` represents a host integer rather than a byte or wire
format.

Fields remain unsigned raw values. Numeric codecs, scaling, units, semantic SSM
enumerations, Label parsing or bit reversal, byte serialization, endian and wire
order conversion, transport integrations, and installation/package exports are
outside the current scope.

## Development style

C++ code follows the Google C++ Style Guide with project-specific exceptions
for C++23 and the `.hpp` public-header extension. Source comments and public API
documentation are written in English. Run `clang-format --style=Google` before
review.

## License

This project is licensed under the BSD 2-Clause License. See [LICENSE](LICENSE).
