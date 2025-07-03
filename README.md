# CHIMERA - Post-Quantum Polymorphic Steganography

**Quantum-Safe, Adaptive Covert-Channel Framework**

A polymorphic framework designed to create practically undetectable, hidden communication channels by disguising data as benign network traffic.

## ⚠️ Project Status

**This project represents one week of learning and coding in this domain - it's my first real attempt at steganographic communication systems. I'm not working on it 24/7, so if you have any observations or feedback, please email me at halaszoliver45(at)gmail.com**

**No guarantees are provided - this is experimental educational software developed during a week-long deep dive into DNS steganography and post-quantum cryptography.**

## What This Project Will Become

CHIMERA aims to be a comprehensive steganographic communication system that:
- Embeds data in DNS traffic (DoH/DoT channels) to appear as encrypted HTTPS traffic
- Implements post-quantum cryptography with TLS 1.3 hybrid key exchange (X25519+ML-KEM768)
- Dynamically switches between DoH, DoT, and UDP/53 transport layers with behavioral timing profiles
- Achieves 3 Mbps throughput with <1% IDS false positive rate

## Current Implementation Status (Week 1 Results)

### ✅ What Currently Works

- **Robust Base64 encoding/decoding** - Complete implementation with proper padding handling
- **DNS packet building** - Structured packet construction with random IDs and proper headers
- **UDP socket communication** - Reliable DNS query sending with timeout support
- **Client configuration system** - Flexible configuration with validation
- **CLI interface** - Command-line argument parsing and file input support
- **Comprehensive testing** - Unit tests, benchmarks, and edge case handling
- **Build system** - CMake configuration with CI/CD pipeline and platform detection
- **Random subdomain generation** - Dynamic domain generation for steganographic purposes
- **Error handling** - Proper error propagation using `std::expected`

### ❌ What Doesn't Work Yet

- **DNS response parsing** - Only sending queries, no response handling
- **TLS/DoH/DoT support** - Only basic UDP DNS implemented
- **Post-quantum cryptography** - No ML-KEM or advanced crypto integration yet
- **Behavioral mimicry** - No timing profiles or traffic analysis evasion
- **Steganographic optimization** - Data encoding could be more sophisticated
- **Async I/O** - Currently synchronous, needs liburing/Boost.Asio integration
- **Proper DNS compression** - Name compression not implemented
- **Interactive sessions** - No bidirectional communication yet

## Architecture Overview

The project is structured with:
- **Core library** (`chimera_core`) with modular client functionality
- **Demo application** for testing and demonstration
- **Comprehensive test suite** with benchmarking capabilities
- **Flexible configuration system** with validation
- **Platform-aware build system** supporting Linux, macOS, and Windows

**Note: Comments are in Hungarian as it's easier for me to work with during this learning phase.**

## One Week Learning Journey

This codebase represents my first week diving deep into:
- **DNS protocol internals** - Understanding packet structure, record types, and encoding
- **Network steganography** - Learning covert channel techniques and detection avoidance
- **Modern C++ practices** - Implementing with C++23 features, `std::expected`, and proper RAII
- **Post-quantum cryptography concepts** - Researching ML-KEM and hybrid key exchange
- **Build system design** - Creating cross-platform CMake configurations
- **Testing methodologies** - Implementing comprehensive test coverage

The code reflects this learning process - you'll see evolution from basic implementations to more sophisticated approaches as I gained understanding throughout the week.

## Building

```
# Basic build
mkdir build && cd build
cmake ..
make -j$(nproc)

# Debug build with verbose output
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Release build with optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Running

```
# Run demo with default message
./build/chimera_demo

# Run with custom message from file
./build/chimera_demo message.txt

# Run with custom DNS server
./build/chimera_demo -s 1.1.1.1 -d test.com message.txt

# Run comprehensive tests
./build/chimera_test

# Run specific test targets
make run_tests
make run_demo
```


## Contributing

**Contributions are especially welcome given this is a learning project!** If you have suggestions, improvements, or educational feedback:

1. **Feel free to submit a pull request** - I'm eager to learn from more experienced developers
2. **Open an issue for discussion** - Especially for architectural decisions or security considerations
3. **Email me directly** at halaszoliver45(at)gmail.com for mentoring or guidance

Since this represents my first week in this domain, I especially appreciate:
- **Code review and best practices feedback** - Help me learn proper patterns
- **Security considerations and threat modeling input** - Critical for steganographic systems
- **Performance optimization suggestions** - Especially for network and crypto operations
- **Documentation improvements** - Help make the codebase more accessible
- **Educational resources** - Papers, books, or tutorials you'd recommend
- **Architecture guidance** - How to structure complex network security projects

## Technical Stack

- **Language**: C++23 (GCC 14 / Clang 18)
- **I/O**: Native sockets (liburing 2.x planned for Linux, Boost.Asio for cross-platform)
- **Cryptography**: OpenSSL 3.2+ with oqs-provider (planned), libsodium 1.0.19 (planned)
- **Build**: CMake 3.15+ with GitHub Actions CI/CD
- **Testing**: Custom test framework (migrating to Catch2 or Google Test)
- **Documentation**: Markdown with planned Doxygen integration

## License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.

### Why Apache 2.0?

Apache 2.0 was chosen for this educational project because:
- **Permissive licensing** - Allows both commercial and non-commercial use
- **Patent protection** - Provides explicit patent grants from contributors
- **Attribution requirements** - Ensures proper credit while remaining flexible
- **Industry standard** - Widely adopted and understood in the open source community
- **Educational friendly** - Encourages learning, modification, and distribution

## Acknowledgments

Thanks to the open-source community and educational resources that made this week-long learning journey possible. Special appreciation for the DNS and cryptography communities whose documentation and examples were invaluable.

---

*This project represents one week of intensive learning and coding in network steganography. Expect rapid evolution as understanding deepens and more sophisticated techniques are implemented.*

**Week 1 Complete** ✅ | **Next Goals (Likely 2-4 weeks)**: DoH/DoT implementation and crypto integration
