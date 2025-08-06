# Contributing

Status
Experimental; code and docs reflect current headers/CMake.

Setup
```bash
git clone ... && cd chimera
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build -j
cmake --build build --target run_tests
```

Code style
- C++20, RAII, smart pointers, const-correctness
- tl::expected for errors; avoid exceptions for flow
- Names: PascalCase types, camelCase functions/vars, UPPER_SNAKE macros, trailing _ for privates
- Includes: std, third-party, then project; no using namespace in headers
- Format: clang-format; lint with clang-tidy if available

Where to edit
- Public headers: include/chimera/
- Sources: src/
- Tests: tests/test_unified.cpp
- Docs: wiki/

Testing
```bash
cmake --build build --target run_tests
build/chimera_test --all
```

PRs
- Describe why; include tests; update docs (README/wiki)
- Keep secrets out of code/logs

Areas
- macOS build fixes (curl/OpenSSL discovery)
- Transport robustness; diagnostics; config wizard UX
- Docs and examples
