# Frequently Asked Questions (FAQ)

## What is Chimera?
A C++20 framework that hides data in DNS traffic (UDP/DoH/DoT) with
libsodium + liboqs cryptography and steganographic encoding.

## What platforms are supported?
Linux and macOS are primary targets. Windows may work with extra setup.

## Which dependencies are required?
libsodium, liboqs, OpenSSL, libcurl, zlib, CMake 3.16+, C++20 compiler.

## How do I build?
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j
```

## How do I run tests?
```bash
cmake --build build --target run_tests
# or
build/chimera_test --all
```

## How do I configure quickly?
Use the configuration wizard:
```cpp
chimera::ConfigWizard w;
auto cfg = w.runInteractiveSetup();
```

## How do I diagnose my system?
```cpp
std::cout << chimera::SystemDiagnostics::generateDetailedReport();
```

## Which transport should I use?
- UDP: fastest, least stealth
- DoH: best stealth, needs HTTPS
- DoT: encrypted, identifiable as TLS on 853

## Message size limits?
Keep payloads small; multi-record + compression improves capacity. For
large data, split and send in chunks.

## macOS curl/OpenSSL issues?
Pass Homebrew prefixes via CMAKE_PREFIX_PATH and set SDK via
CMAKE_OSX_SYSROOT.
