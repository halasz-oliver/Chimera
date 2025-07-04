# CHIMERA - Post-Quantum Polymorphic Steganography

**Quantum-Safe, Adaptive Covert-Channel Framework**

A polymorphic framework designed to create practically undetectable, hidden communication channels by disguising data as benign network traffic.

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

### ✅ What Currently Works

- **Robust Base64 encoding/decoding**: A complete implementation with proper padding handling.
- **DNS Packet Building**: Structured packet construction with random IDs and proper headers.
- **UDP Socket Communication**: Basic DNS query sending over UDP with timeout support.
- **Client Configuration System**: Flexible configuration setup with validation.
- **CLI Interface**: Command-line argument parsing and file input support for the demo application.
- **Build System**: A cross-platform CMake configuration with a CI/CD pipeline defined in `ci.yml`.
- **Random Subdomain Generation**: Dynamic domain generation for steganographic queries.
- **AEAD Cryptography Layer**: A foundational crypto module using **libsodium** to provide **ChaCha20-Poly1305** for authenticated encryption. This completes a major part of the crypto core.
- **Comprehensive Testing**: A basic test suite covering core functionalities and a new suite for the AEAD crypto layer.

### 🚧 What's In Progress

- **Phase 1: Cryptographic Core**: The AEAD layer is complete. The next step is integrating a TLS 1.3 library to implement the planned hybrid (PQC + classical) key exchange.

### ❌ What Doesn't Work Yet

- **DNS Response Parsing**: The system only sends queries; it does not yet handle or parse responses.
- **TLS/DoH/DoT Support**: Only basic UDP/53 DNS is implemented. Secure transports are next on the roadmap.
- **Post-Quantum Cryptography**: The high-level PQC primitives (like ML-KEM) are not yet integrated.
- **Behavioral Mimicry**: No advanced timing profiles or traffic analysis evasion techniques are implemented.
- **Asynchronous I/O**: Communication is currently synchronous. Integration of `io_uring` or `Boost.Asio` is planned for high-performance I/O.

## Getting Started

### Prerequisites

Before you begin, ensure you have the following dependencies installed on your system.

1.  **C++23 Compiler**: GCC (version 12+) or Clang (version 15+).
2.  **CMake**: Version 3.16 or higher is required.
3.  **Libsodium**: The core cryptography library.

#### Installing Libsodium

-   **On Debian/Ubuntu:**
    ```
    sudo apt-get update && sudo apt-get install -y libsodium-dev
    ```
-   **On macOS (using Homebrew):**
    ```
    brew install libsodium
    ```
-   **On Arch Linux:**
    ```
    sudo pacman -S libsodium
    ```
-   **From Source:**
    If your package manager doesn't have it, you can compile it from source by following the official [libsodium installation instructions](https://libsodium.gitbook.io/doc/installation).

### Building the Project

```
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

```
# Run the demo application with a default message
./build/chimera_demo

# Run the demo with a message from a file
./build/chimera_demo message.txt

# Run with a custom DNS server and domain
./build/chimera_demo -s 1.1.1.1 -d my-covert-domain.com message.txt

# Run the comprehensive test suite
./build/chimera_test
# or use the custom make target
make run_tests
```

## Architecture Overview

The project is structured for modularity and testability:
- **Core Library (`chimera_core`)**: Contains all core logic, including the client, DNS packet handling, and the new crypto module.
- **Demo Application (`chimera_demo`)**: A command-line executable to showcase and test the library's functionality.
- **Test Suite (`chimera_test`)**: A separate executable for running unit, integration, and benchmark tests.

**Note**: Source code comments are primarily in Hungarian, as it's my native language and helps me think through complex problems more efficiently during this learning phase.

## Technical Stack

- **Language**: C++23
- **Build**: CMake (3.16+)
- **CI/CD**: GitHub Actions
- **Cryptography**:
    - **libsodium 1.0.19+**: For the core AEAD encryption layer (ChaCha20-Poly1305).
    - **OpenSSL 3.2+ with oqs-provider**: Planned for the TLS 1.3 hybrid post-quantum key exchange.
- **I/O**: Native sockets, with `io_uring` (Linux) and `Boost.Asio` (cross-platform) planned for future async implementation.
- **Testing**: A custom test runner, with plans to migrate to Catch2 or Google Test.

## Project Roadmap

The development is planned in distinct phases:

1.  **Phase 1: Cryptographic Core**
    -   [x] Libsodium-based AEAD abstraction (`ChaCha20-Poly1305`)
    -   [ ] TLS 1.3 hybrid key exchange implementation (X25519 + ML-KEM768)

2.  **Phase 2: Transport Layer**
    -   [ ] Abstract transport classes (`TransportDoH`, `TransportDoT`, `TransportUDP`)
    -   [ ] `io_uring` wrapper for high-performance I/O on Linux

3.  **Phase 3: Steganographic Encoding**
    -   [ ] Advanced DNS codec supporting A, AAAA, and TXT records
    -   [ ] HTTP/2 body-based encoding for DoH transport

4.  **Phase 4: Behavioral Mimicry**
    -   [ ] Implement behavioral profiles (e.g., "web browsing," "cloud sync")
    -   [ ] Load timing patterns from external configuration files (e.g., YAML)

5.  **Phase 5: Integration & CLI**
    -   [ ] Enhance the `chimera-cli` with send/receive and diagnostic tools
    -   [ ] Create a Docker Compose environment for testing and simulation

6.  **Phase 6 & 7: Formal Verification & Security Audit**
    -   [ ] Use TLA+ for formal model checking of the handshake protocol
    -   [ ] Integrate static analysis and fuzz testing (`clang-tidy`, `libFuzzer`)

## Contributing

**Contributions are especially welcome, as this is a learning project!** If you have suggestions, improvements, or educational feedback:

1.  **Feel free to submit a pull request** - I'm eager to learn from more experienced developers.
2.  **Open an issue for discussion** - Especially for architectural decisions or security considerations.
3.  **Email me directly** for mentoring or guidance.

I especially appreciate:
-   Code reviews and best practices feedback.
-   Security considerations and threat modeling input.
-   Performance optimization suggestions.
-   Recommendations for educational resources (papers, books, tutorials).

## License

This project is licensed under the Apache License 2.0. See the `LICENSE` file for details. It was chosen because it is permissive, provides patent protection, has clear attribution requirements, and is an industry standard that encourages learning and modification.

## Acknowledgments

Thanks to the open-source community and the creators of the invaluable documentation and examples in the DNS and cryptography fields that have made this learning journey possible.

---

*This project represents an intensive, ongoing learning process in network steganography. Expect rapid evolution as understanding deepens and more sophisticated techniques are implemented.*