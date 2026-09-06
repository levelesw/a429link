# Repository Guidelines

## Build and compatibility

- Use C++23.
- Keep the library header-only and free of external runtime dependencies unless an Issue explicitly changes this policy.
- Preserve the public CMake target `a429link::a429link`.
- Specify the language level with target-scoped
  `target_compile_features(... cxx_std_23)`.
- Disable compiler extensions.

## Coding style

- Follow the Google C++ Style Guide, with these project-specific exceptions:
  - C++23 features are allowed.
  - Public headers use the `.hpp` extension.
  - Write source-code comments and public API documentation in English.
  - Use comments to explain non-obvious constraints and rationale, not to restate the code.
- Test cases may use concise `Purpose:` and `Coverage:` comments immediately
  before the test definition to make the tested intent and covered contract
  visible at a glance. Do not narrate individual assertions or implementation
  steps.
- Format modified C++ files using `clang-format --style=Google`.
- Use PascalCase for functions, `kPascalCase` for enumerators, and
  `snake_case` for accessors.
- Add the project copyright notice and
  `SPDX-License-Identifier: BSD-2-Clause` to new source and build files.

## Architecture

- Keep the generic 32-bit word representation independent of ARINC 429-specific
  fields, codecs, serialization, and transports.
- Do not add public APIs solely to make testing easier.

## Version control

- Start every commit subject with an appropriate Gitmoji emoji.

## Verification

For changes affecting the C++ library, run:

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
git diff --check
```
