# Chimera Framework Wiki

Welcome to Chimera. This wiki reflects the current codebase (C++20,
CMake, libsodium/liboqs/libcurl/OpenSSL/zlib).

## Quick links
- Getting-Started.md
- Basic-Usage.md
- Configuration.md
- API-Reference.md
- Advanced-Features.md
- System-Diagnostics.md
- Troubleshooting.md
- FAQ.md

## Summary
- Transports: UDP, DoH, DoT
- Steganography: single or multi-record encoding, compression, noise,
  fragment limits
- Wizard: interactive, validated, no emojis
- Diagnostics: crypto libs, DNS sanity, resource usage, permissions;
  timestamped report

## Build/test
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j
cmake --build build --target run_tests
```

## Requirements
- C++20, CMake 3.16+
- libs: libsodium, liboqs, OpenSSL, libcurl, zlib

## Next
Read Getting-Started.md, then Configuration.md for full ClientConfig
options.
