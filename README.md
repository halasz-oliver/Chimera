# CHIMERA - Post-Quantum Polymorphic Steganography

Quantum-safe, adaptive covert-channel framework.

> [!WARNING]
> **VERY JANK, VERY MUCH MADE JUST FOR LEARNING!**
> **PLEASE DON'T USE IT IN ACTUAL PROJECTS!**
>
> ---
>
> `Version: 0.2.3`
>
> `Status: Experimental`

## Goals
- DNS-based steganographic transport (UDP/DoT/DoH)
- Post-quantum crypto via liboqs + classical via libsodium
- Behavioral mimicry, capacity estimation, fragmentation, compression

## Quick start
```bash
# Linux (Debian/Ubuntu)
sudo apt update && \
  sudo apt install -y cmake build-essential libsodium-dev liboqs-dev \
  libssl-dev libcurl4-openssl-dev zlib1g-dev
# macOS
xcode-select --install || true
brew install cmake libsodium liboqs openssl curl zlib

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
# tests
cmake --build build --target run_tests
# demo
build/chimera_demo
```

## Configuration wizard
```cpp
#include "chimera/config_wizard.hpp"
chimera::ConfigWizard wizard;
auto cfg = wizard.runInteractiveSetup();
```
Prompts include DNS server, target domain, transport (DoH/DoT/UDP),
encoding (single/multi), compression (y/n), and noise ratio [0..1].
Inputs are validated for IP/hostname and domain format.

## System diagnostics
```cpp
#include "chimera/system_diagnostics.hpp"
auto reports = chimera::SystemDiagnostics::runPreflightChecks();
auto text = chimera::SystemDiagnostics::generateDetailedReport();
```
Checks include crypto libraries presence (libsodium, liboqs), DNS
resolution sanity, resource usage thresholds, and basic exec permissions.
Cross-platform friendly; logs as [CHIMERA LEVEL] messages.

## Build/test targets
- Library: chimera_core; Demo: chimera_demo; Tests: chimera_test
- CMake custom targets: run_tests, run_core_tests, run_transport_tests,
  run_steganography_tests, run_quick_tests, run_performance_tests,
  run_all_tests
- Single test: run chimera_test directly and pass your filter flag if
  implemented in tests/test_unified.cpp

## Notes
- Requires: CMake 3.16+, C++20, libs: libsodium, OpenSSL, liboqs, libcurl,
  zlib
- On macOS, specify SDK if CMake fails to find curl: set
  CMAKE_OSX_SYSROOT and/or CMAKE_PREFIX_PATH to Homebrew curl
- See wiki/ for deeper guides; this README reflects current code and
  CMakeLists.
