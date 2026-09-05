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

## Minimal example

```cpp
#include <a429link/field.hpp>

#include <cstdint>

int main() {
  using EquipmentId = a429link::Field<9U, 8U>;

  const auto word = a429link::Word::FromRaw(0x1234'5678U);

  // Public bit positions are 1-origin. Replace raw bits 8..15.
  const auto updated = EquipmentId::WithValue(word, 0xABU);
  if (!updated.has_value()) {
    return 1;
  }

  return EquipmentId::Extract(updated.value()) == 0xABU ? 0 : 1;
}
```

`Field<Lsb, Width>` defines a reusable bit range at compile time. Invalid ranges
are rejected by its template constraints. `WithValue()` returns a
`std::expected` error when a value does not fit the configured width and returns
a new `Word` on success; it does not modify the original value.

For ranges selected at runtime, `Word::Extract()` and `Word::WithField()` accept
an LSB and width directly and report invalid ranges with `std::expected`.

## Current scope

This initial slice provides the immutable `a429link::Word` 32-bit value type,
raw-value conversion, validated extraction and replacement of contiguous bit
fields, and the compile-time configured `a429link::Field` helper. Bit positions
are 1-origin, and `raw()` represents a host integer value rather than a byte or
wire format.

A429-specific layouts and meanings such as Label, SDI, data fields, SSM, and
Parity are not defined. Numeric codecs, parity handling, byte serialization,
endian conversion, transport integrations, installation/package exports, and
CI/CD are also outside the current scope.

## Development style

C++ code follows the Google C++ Style Guide with project-specific exceptions
for C++23 and the `.hpp` public-header extension. Source comments and public API
documentation are written in English. Run `clang-format --style=Google` before
review.

## License

This project is licensed under the BSD 2-Clause License. See [LICENSE](LICENSE).
