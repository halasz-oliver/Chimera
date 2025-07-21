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

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/your-username/chimera.git
cd chimera
```

### 2. Install Dependencies

#### macOS (using Homebrew)
```bash
brew install libsodium liboqs curl openssl cmake
```

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install libsodium-dev liboqs-dev libcurl4-openssl-dev libssl-dev cmake build-essential
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

Run the test suite to ensure everything is working:

```bash
# Run basic compatibility tests
./chimera_test

# Run comprehensive Phase 2 tests
./chimera_test_comprehensive
```

Expected output:
```
=== CHIMERA Comprehensive Test Suite ===
Testing Phase 1 + Phase 2 functionality
Production-ready steganographic framework

PHASE 1 TESTS
[TEST] Base64 Comprehensive...
[PASS] Base64 Comprehensive passed
[TEST] Cryptography Production...
[PASS] Cryptography Production passed
...

CHIMERA PHASE 1 + 2 FULLY VALIDATED!
[OK] Quantum-resistant cryptography (X25519 + ML-KEM768)
[OK] Multi-transport steganography (UDP/DoH/DoT)
[OK] Behavioral mimicry and evasion
[OK] High-performance async I/O
[OK] Production-ready steganographic framework
```

## First Test

Try sending your first message:

```bash
# Basic UDP message
./chimera_demo "Hello from CHIMERA!"

# Using DoH transport
./chimera_demo --transport doh "Secure message via HTTPS"

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