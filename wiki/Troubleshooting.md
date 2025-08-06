# Troubleshooting

## Installation
- Install: libsodium, liboqs, OpenSSL, libcurl, zlib, cmake, compiler
- macOS: use Homebrew and pass CMAKE_PREFIX_PATH for curl/openssl if
  needed

## Build
```bash
rm -rf build && cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && \
cmake --build build -j
```
If CMake can’t find curl on macOS:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$(brew --prefix curl);$(brew --prefix openssl)" \
  -DCMAKE_FIND_FRAMEWORK=LAST -DCMAKE_OSX_SYSROOT=$(xcrun --sdk macosx --show-sdk-path)
```

## Runtime
- DNS issues: nslookup example.com 8.8.8.8
- Libraries missing: check pkg-config --libs libsodium liboqs
- Timeouts: increase config.timeout

## DoH/DoT
- Test endpoints: curl -v https://cloudflare-dns.com/dns-query; openssl
  s_client -connect 1.1.1.1:853
- Switch transports via config.transport (UDP/DoH/DoT)

## Diagnostics
```cpp
std::cout << chimera::SystemDiagnostics::generateDetailedReport();
```

## Tests
```bash
cmake --build build --target run_tests
build/chimera_test --all
```

## Tips
- Use UDP locally; switch to DoH/DoT for stealth
- Ensure compiler supports C++20
- Verify liboqs is built with desired KEMs if needed
