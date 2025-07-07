# CHIMERA - Post-Quantum Polymorphic Steganography

**Quantum-Safe, Adaptive Covert-Channel Framework**

A polymorphic framework designed to create practically undetectable, hidden communication channels by disguising data as benign network traffic with post-quantum cryptographic security.

## ⚠️ Project Status

**This project represents an ongoing learning and coding journey into steganographic communication systems. It is experimental, educational software. No guarantees are provided.**

I'm not working on it 24/7, so if you have any observations, feedback, or want to contribute, please email me at `halaszoliver45(at)gmail.com`.

## Project Goals

CHIMERA aims to be a comprehensive steganographic communication system that:
- **Embeds data in DNS traffic** (DoH/DoT channels) to appear as standard encrypted HTTPS traffic.
- **Implements post-quantum cryptography** with a TLS 1.3 hybrid key exchange (X25519+ML-KEM768).
- **Dynamically switches between transport layers** (DoH, DoT, UDP/53) using behavioral timing profiles to evade detection.
- Aims for a **3 Mbps throughput** with a false positive rate of less than 1% on intrusion detection systems.

## Current Implementation Status

### ✅ What Currently Works (Phase 1 Complete!)

- **Robust Base64 encoding/decoding**: Complete implementation with proper padding handling.
- **DNS Packet Building**: Structured packet construction with random IDs and proper headers.
- **UDP Socket Communication**: Basic DNS query sending over UDP with timeout support.
- **Client Configuration System**: Flexible configuration setup with validation.
- **CLI Interface**: Command-line argument parsing and file input support for the demo application.
- **Build System**: Cross-platform CMake configuration with CI/CD pipeline defined in `ci.yml`.
- **Random Subdomain Generation**: Dynamic domain generation for steganographic queries.
- **AEAD Cryptography Layer**: Complete crypto module using **libsodium** providing **ChaCha20-Poly1305** for authenticated encryption.
- **Hybrid Key Exchange Framework**: **NEW!** Complete API and structure for TLS 1.3 hybrid key exchange (X25519 + ML-KEM768) with real X25519 implementation.
- **Key Derivation**: HKDF-based key derivation from combined classical and post-quantum secrets.
- **Professional Codebase**: Updated with English-only user interface and professional Hungarian comments.
- **Comprehensive Testing**: Test suite covering all core functionalities including hybrid cryptography.

### 🚧 What's In Progress

- **ML-KEM768 Integration**: The hybrid key exchange structure is complete, but currently uses a placeholder ML-KEM768 implementation. Integration with **liboqs**, **PQClean**, or **Kyber reference implementation** is the next priority.

### ❌ What Doesn't Work Yet

- **DNS Response Parsing**: The system only sends queries; it does not yet handle or parse responses.
- **TLS/DoH/DoT Support**: Only basic UDP/53 DNS is implemented. Secure transports are next on the roadmap.
- **Production-Ready Post-Quantum Cryptography**: ML-KEM768 placeholder needs replacement with real implementation.
- **Behavioral Mimicry**: No advanced timing profiles or traffic analysis evasion techniques are implemented.
- **Asynchronous I/O**: Communication is currently synchronous. Integration of `io_uring` or `Boost.Asio` is planned for high-performance I/O.

## Getting Started

### Prerequisites

Before you begin, ensure you have the following dependencies installed on your system.

1.  **C++23 Compiler**: GCC (version 12+) or Clang (version 15+).
2.  **CMake**: Version 3.16 or higher is required.
3.  **Libsodium**: The core cryptography library.
4.  **tl::expected**: Header-only library for error handling (included in project).

#### Installing Libsodium

-   **On Debian/Ubuntu:**
    ```bash
    sudo apt-get update && sudo apt-get install -y libsodium-dev
    ```
-   **On macOS (using Homebrew):**
    ```bash
    brew install libsodium
    ```
-   **On Arch Linux:**
    ```bash
    sudo pacman -S libsodium
    ```
-   **From Source:**
    If your package manager doesn't have it, you can compile it from source by following the official [libsodium installation instructions](https://libsodium.gitbook.io/doc/installation).

### Building the Project

```bash
# 1. Clone the repository
git clone https://github.com/your-username/chimera.git
cd chimera

# 2. Create a build directory
mkdir build && cd build

# 3. Configure with CMake
# For a standard release build:
cmake .. -DCMAKE_BUILD_TYPE=Release

# For a debug build:
cmake .. -DCMAKE_BUILD_TYPE=Debug

# 4. Compile the code
# Using make (adjust -j for your number of CPU cores)
make -j$(nproc)

# Using Ninja for potentially faster builds
# cmake -G Ninja ..
# ninja
```

## Running

After a successful build, the executables will be in the `build/` directory.

```bash
# Run the demo application with a default message
./build/chimera_demo

# Run the demo with a message from a file
./build/chimera_demo message.txt

# Run with a custom DNS server and domain
./build/chimera_demo -s 1.1.1.1 -d my-covert-domain.com message.txt

# Run the comprehensive test suite (including new crypto tests)
./build/chimera_test
# or use the custom make target
make run_tests
```

### Command Line Options

- `-s, --server`: DNS server address (default: 8.8.8.8)
- `-p, --port`: DNS server port (default: 53)
- `-d, --domain`: Target domain (default: example.com)
- `-t, --timeout`: Timeout in milliseconds (default: 5000)
- `-h, --help`: Show help message

## Architecture Overview

The project is structured for modularity and testability:

- **Core Library (`chimera_core`)**: Contains all core logic including:
    - **Client Module**: High-level steganographic communication interface
    - **DNS Packet Module**: Low-level DNS protocol handling
    - **Crypto Module**: **NEW!** Hybrid cryptographic primitives (AEAD + Key Exchange)
- **Demo Application (`chimera_demo`)**: Command-line executable to showcase and test the library's functionality
- **Test Suite (`chimera_test`)**: Comprehensive testing including crypto validation and benchmarks

### Cryptographic Architecture

CHIMERA implements a **defense-in-depth** cryptographic approach:

1. **Hybrid Key Exchange** (X25519 + ML-KEM768):
    - Classical X25519 ECDH for current security
    - ML-KEM768 for post-quantum resistance
    - Combined secrets via HKDF for maximum security

2. **AEAD Encryption** (ChaCha20-Poly1305):
    - Authenticated encryption with associated data
    - Protects against tampering and replay attacks
    - Proven security with high performance

**Note**: Source code comments are primarily in Hungarian, as it's my native language and helps me think through complex problems more efficiently during this learning phase.

## Technical Stack

- **Language**: C++23
- **Build**: CMake (3.16+)
- **CI/CD**: GitHub Actions
- **Cryptography**:
    - **libsodium 1.0.19+**: For AEAD encryption layer (ChaCha20-Poly1305) and X25519 key exchange
    - **ML-KEM768**: Placeholder implementation (production requires liboqs/PQClean integration)
    - **HKDF**: Key derivation from hybrid secrets
- **Error Handling**: `tl::expected` for functional error management
- **I/O**: Native sockets, with `io_uring` (Linux) and `Boost.Asio` (cross-platform) planned for future async implementation
- **Testing**: Custom test runner with plans to migrate to Catch2 or Google Test

## Project Roadmap

The development is planned in distinct phases:

1.  **Phase 1: Cryptographic Core** ✅ **COMPLETE**
    -   [x] Libsodium-based AEAD abstraction (`ChaCha20-Poly1305`)
    -   [x] Hybrid key exchange framework (X25519 + ML-KEM768 structure)
    -   [x] Real X25519 implementation via libsodium
    -   [x] HKDF key derivation
    -   [ ] **In Progress**: Replace ML-KEM768 placeholder with production implementation

2.  **Phase 2: Transport Layer**
    -   [ ] Abstract transport classes (`TransportDoH`, `TransportDoT`, `TransportUDP`)
    -   [ ] TLS 1.3 integration for DoH/DoT
    -   [ ] `io_uring` wrapper for high-performance I/O on Linux

3.  **Phase 3: Steganographic Encoding**
    -   [ ] Advanced DNS codec supporting A, AAAA, and TXT records
    -   [ ] HTTP/2 body-based encoding for DoH transport
    -   [ ] DNS response parsing and bidirectional communication

4.  **Phase 4: Behavioral Mimicry**
    -   [ ] Implement behavioral profiles (e.g., "web browsing," "cloud sync")
    -   [ ] Load timing patterns from external configuration files (e.g., YAML)
    -   [ ] Traffic analysis evasion techniques

5.  **Phase 5: Integration & CLI**
    -   [ ] Enhance the `chimera-cli` with send/receive and diagnostic tools
    -   [ ] Create a Docker Compose environment for testing and simulation
    -   [ ] Configuration file support (JSON/YAML)

6.  **Phase 6 & 7: Formal Verification & Security Audit**
    -   [ ] Use TLA+ for formal model checking of the handshake protocol
    -   [ ] Integrate static analysis and fuzz testing (`clang-tidy`, `libFuzzer`)
    -   [ ] Third-party security audit of cryptographic implementation

## Security Considerations

- **Hybrid Cryptography**: Protects against both classical and quantum attacks
- **Forward Secrecy**: Each session uses ephemeral keys
- **Authentication**: AEAD prevents message tampering and forgery
- **Randomization**: Cryptographically secure random number generation
- **Side-Channel Resistance**: Built on libsodium's proven implementations

**Important**: The current ML-KEM768 implementation is a placeholder for structural demonstration. **Do not use in production** until replaced with a verified implementation.

## Contributing

**Contributions are especially welcome, as this is a learning project!** If you have suggestions, improvements, or educational feedback:

1.  **Feel free to submit a pull request** - I'm eager to learn from more experienced developers.
2.  **Open an issue for discussion** - Especially for architectural decisions or security considerations.
3.  **Email me directly** for mentoring or guidance.

I especially appreciate:
-   Code reviews and best practices feedback
-   Security considerations and threat modeling input
-   Performance optimization suggestions
-   Recommendations for educational resources (papers, books, tutorials)
-   **ML-KEM768 integration assistance** (top priority!)

## License

This project is licensed under the Apache License 2.0. See the `LICENSE` file for details. It was chosen because it is permissive, provides patent protection, has clear attribution requirements, and is an industry standard that encourages learning and modification.

## Acknowledgments

Thanks to the open-source community and the creators of the invaluable documentation and examples in the DNS and cryptography fields that have made this learning journey possible. Special thanks to the libsodium project for providing a robust, secure cryptographic foundation.

*This project represents an intensive, ongoing learning process in network steganography and post-quantum cryptography. Phase 1 represents a significant milestone in implementing hybrid cryptographic systems. Expect continued rapid evolution as understanding deepens and more sophisticated techniques are implemented.*# CI/CD Test
