# Getting Started

This guide will help you install, build, and run CHIMERA for the first time.

## Prerequisites

### System Requirements
- **Operating System**: macOS, Linux, or Windows
- **Compiler**: C++17 compatible (GCC 8+, Clang 10+, MSVC 2019+)
- **CMake**: Version 3.15 or later
- **Git**: For cloning the repository

### Dependencies

CHIMERA requires several libraries for cryptography and networking:

#### Required Libraries
- **libsodium**: For AEAD encryption (ChaCha20-Poly1305)
- **liboqs**: For post-quantum cryptography (ML-KEM768)
- **libcurl**: For DNS-over-HTTPS transport
- **OpenSSL**: For DNS-over-TLS transport
- **zlib**: For payload compression (Phase 3)

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/halasz-oliver/chimera.git
cd chimera
```

### 2. Install Dependencies

#### macOS (using Homebrew)
```bash
brew install libsodium liboqs curl openssl cmake zlib
```

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install libsodium-dev liboqs-dev libcurl4-openssl-dev libssl-dev cmake build-essential zlib1g-dev
```

#### CentOS/RHEL/Fedora
```bash
# Fedora
sudo dnf install libsodium-devel liboqs-devel libcurl-devel openssl-devel cmake gcc-c++

# CentOS/RHEL (may need EPEL repository)
sudo yum install libsodium-devel libcurl-devel openssl-devel cmake gcc-c++
# Note: liboqs may need to be built from source on older distributions
```

### 3. Build CHIMERA

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
make -j$(nproc)
```

### 4. Verify Installation

# Run the unified test suite to ensure everything is working:

```bash
# Run all tests (recommended)
./chimera_test --all

# Or run specific test categories:
./chimera_test --core              # Core functionality only
./chimera_test --transport         # Transport layer only  
./chimera_test --steganography     # Phase 3 features only
./chimera_test --integration       # Integration tests only
./chimera_test --performance       # Performance benchmarks only

# Quick essential tests
./chimera_test --quick

# Help and usage
./chimera_test --help
```

Expected output:
```
=== CHIMERA Unified Test Suite ===
Production-ready steganographic framework testing

CORE FUNCTIONALITY TESTS (Phase 1)
[TEST] Base64 Encoding/Decoding...
[PASS] Base64 Encoding/Decoding passed
[TEST] AEAD Cryptography...
[PASS] AEAD Cryptography passed
[TEST] Hybrid Key Exchange (X25519 + ML-KEM768)...
[PASS] Hybrid Key Exchange (X25519 + ML-KEM768) passed
[TEST] DNS Packet Construction...
[PASS] DNS Packet Construction passed

TRANSPORT LAYER TESTS (Phase 2)
[TEST] Transport Layer Abstraction...
[PASS] Transport Layer Abstraction passed
[TEST] Behavioral Mimicry...
[PASS] Behavioral Mimicry passed
[TEST] Async I/O Framework...
[PASS] Async I/O Framework passed

STEGANOGRAPHIC ENHANCEMENT TESTS (Phase 3)
[TEST] Multi-record DNS Encoding...
[PASS] Multi-record DNS Encoding passed
[TEST] IPv4/IPv6 Address Encoding...
[PASS] IPv4/IPv6 Address Encoding passed
[TEST] Enhanced TXT Record Encoding...
[PASS] Enhanced TXT Record Encoding passed
[TEST] HTTP/2 Body Encoding...
[PASS] HTTP/2 Body Encoding passed
[TEST] Capacity Estimation...
[PASS] Capacity Estimation passed
[TEST] Fragment Management...
[PASS] Fragment Management passed

INTEGRATION TESTS
[TEST] End-to-End Steganographic Flow...
[PASS] End-to-End Steganographic Flow passed

PERFORMANCE TESTS
[TEST] Encoding and Transport Performance...
[PASS] Encoding and Transport Performance passed

=== TEST SUMMARY ===
Total: 15/15 tests passed
🎉 ALL TESTS PASSED!

CHIMERA TEST RESULTS:
✅ All 15 tests passed!
🚀 CHIMERA is ready for production use!
```

## First Test

Try sending your first message:

```bash
# Basic UDP message
./chimera_demo "Hello from CHIMERA!"

# Using DoH transport with multi-record encoding
./chimera_demo --transport doh --encoding multi-record "Secure message via HTTPS"

# With compression and noise injection
./chimera_demo --compress --noise 0.15 "Optimized steganographic message"

# Custom target domain
./chimera_demo --domain your-domain.com "Custom domain test"
```

## Docker Installation (Alternative)

If you prefer containerized deployment:

```bash
# Build Docker image
docker build -t chimera .

# Run tests
docker run --rm chimera ./chimera_test_comprehensive

# Interactive shell
docker run -it --rm chimera /bin/bash
```

## Troubleshooting Installation

### Common Issues

#### 1. Missing liboqs
```bash
# If liboqs is not available in your package manager, build from source:
git clone https://github.com/open-quantum-safe/liboqs.git
cd liboqs
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
make -j$(nproc) && sudo make install
```

#### 2. CMake Can't Find Dependencies
```bash
# Specify custom paths if needed
cmake -DCMAKE_PREFIX_PATH="/usr/local;/opt/homebrew" ..
```

#### 3. Compiler Issues
```bash
# Force specific compiler
cmake -DCMAKE_CXX_COMPILER=g++-10 ..
```

#### 4. macOS OpenSSL Issues
```bash
# If using Homebrew OpenSSL
cmake -DOPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl ..
```

### Verification Steps

1. **Check library versions**:
   ```bash
   pkg-config --modversion libsodium  # Should be 1.0.18+
   openssl version                    # Should be 1.1.1+
   ```

2. **Test crypto functionality**:
   ```bash
   ./chimera_test  # Should pass all legacy tests
   ```

3. **Test network connectivity**:
   ```bash
   # Test basic DNS resolution
   nslookup example.com 8.8.8.8
   ```

## Next Steps

Once CHIMERA is installed and working:

1. **[Basic Usage](Basic-Usage.md)** - Learn the fundamental operations
2. **[Configuration](Configuration.md)** - Customize CHIMERA for your needs
3. **[Advanced Features](Advanced-Features.md)** - Explore Phase 2 capabilities

## Development Setup

For developers wanting to contribute:

```bash
# Install additional development tools
brew install clang-format cppcheck  # macOS
sudo apt install clang-format cppcheck  # Ubuntu

# Enable debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)

# Run with debugging
gdb ./chimera_demo
```

## Performance Tuning

For production deployments:

```bash
# Optimized release build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native" ..
make -j$(nproc)
```

---

**Next**: Continue to [Basic Usage](Basic-Usage.md) to learn how to use CHIMERA effectively.
