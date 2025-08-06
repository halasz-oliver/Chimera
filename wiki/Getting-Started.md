# Getting Started

This guide helps you install, build, and run Chimera.

## Prerequisites
- OS: macOS or Linux (Windows not actively tested)
- Compiler: C++20 (GCC 10+/Clang 12+)
- CMake: 3.16+
- Git
- Libraries: libsodium, liboqs, OpenSSL, libcurl, zlib

## Install deps
### macOS (Homebrew)
```bash
xcode-select --install || true
brew install cmake libsodium liboqs openssl curl zlib
```

### Ubuntu/Debian
```bash
sudo apt update
sudo apt install -y cmake build-essential libsodium-dev liboqs-dev \
  libssl-dev libcurl4-openssl-dev zlib1g-dev
```

## Build
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Run
```bash
# demo
build/chimera_demo
# tests (custom CMake targets)
cmake --build build --target run_tests
# or run binary
build/chimera_test --all
```

## macOS notes
If CMake fails to locate curl/openssl headers from the SDK, point to
Homebrew prefixes:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$(brew --prefix curl);$(brew --prefix openssl)" \
  -DCMAKE_FIND_FRAMEWORK=LAST -DCMAKE_OSX_SYSROOT=$(xcrun --sdk macosx --show-sdk-path)
```

## First configuration
```cpp
#include "chimera/config_wizard.hpp"
chimera::ConfigWizard w;
auto cfg = w.runInteractiveSetup();
```
Prompts include DNS server (IPv4/IPv6/hostname), target domain, transport
(DoH/DoT/UDP), encoding (single/multi), compression (y/n), and noise
ratio [0..1].

## Diagnostics
```cpp
#include "chimera/system_diagnostics.hpp"
std::cout << chimera::SystemDiagnostics::generateDetailedReport();
```

## Troubleshooting
- liboqs missing: build from source (see OQS docs)
- curl/openssl not found on macOS: set CMAKE_PREFIX_PATH as above
- DNS issues: verify nslookup example.com works

## Next steps
- Read Basic-Usage.md and Configuration.md for API details
