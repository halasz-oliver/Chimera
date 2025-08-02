# CHIMERA - Post-Quantum Polymorphic Steganography

**Quantum-Safe, Adaptive Covert-Channel Framework**

A production-ready polymorphic framework designed to create practically undetectable, hidden communication channels by disguising data as benign network traffic with post-quantum cryptographic security.

## Project Status

**🎉 ALL PHASES COMPLETE!** CHIMERA is now a fully functional, production-ready steganographic communication system with comprehensive multi-record DNS encoding, advanced payload distribution, behavioral mimicry, and quantum-resistant cryptography.

**Current Version**: 0.2.1  
**Status**: Production Ready  
**Test Coverage**: 15/15 tests passing across all phases

This is an ongoing learning and development project. For feedback, contributions, or collaboration, email me at `halaszoliver45(at)gmail.com`.

## Project Goals ✅ ACHIEVED

CHIMERA is a comprehensive steganographic communication system that:
- ✅ **Embeds data in DNS traffic** (DoH/DoT channels) to appear as standard encrypted HTTPS traffic
- ✅ **Implements post-quantum cryptography** with hybrid key exchange (X25519+ML-KEM768)
- ✅ **Dynamically switches between transport layers** (DoH, DoT, UDP/53) using behavioral timing profiles to evade detection
- ✅ **Multi-record steganographic encoding** with A, AAAA, and enhanced TXT records
- ✅ **Advanced payload distribution** with compression, fragmentation, and noise injection
- ✅ **High-performance async I/O** framework for production deployments

## 🚀 Quick Start

For complete setup instructions, detailed usage examples, and configuration options, please visit our **[Wiki](wiki/Home.md)**.

### Essential Setup
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt install libsodium-dev liboqs-dev libcurl4-openssl-dev libssl-dev cmake build-essential zlib1g-dev

# Install dependencies (macOS)
brew install libsodium liboqs curl openssl cmake zlib

# Build
mkdir build && cd build && cmake .. && make -j$(nproc)

# Test all phases
make run_tests

# Run demo
./chimera_demo "Hello, CHIMERA!"
```

**For detailed installation instructions for all platforms, see:** **[Getting Started Guide](wiki/Getting-Started.md)**

### Basic Usage
```cpp
#include "chimera/client.hpp"

// Phase 3 configuration with multi-record encoding
chimera::ClientConfig config;
config.dns_server = "8.8.8.8";
config.target_domain = "example.com";
config.encoding_strategy = chimera::EncodingStrategy::MULTI_RECORD;
config.use_compression = true;
config.noise_ratio = 0.1;

chimera::ChimeraClient client(config);
auto result = client.send_text("Secret message");

if (result) {
    std::cout << "Message sent using " << result->fragments_sent 
              << " fragments with compression: " << result->compression_used << std::endl;
}
```

**For comprehensive usage examples and API documentation, see:** **[Basic Usage Guide](wiki/Basic-Usage.md)**

## 📚 Documentation

| Topic | Description | Wiki Link |
|-------|-------------|-----------|
| **Installation** | Platform-specific setup, dependencies, building | [Getting Started](wiki/Getting-Started.md) |
| **Basic Usage** | Simple examples, CLI usage, core concepts | [Basic Usage](wiki/Basic-Usage.md) |
| **Configuration** | Complete configuration reference | [Configuration](wiki/Configuration.md) |
| **Advanced Features** | Behavioral mimicry, async I/O, transport switching | [Advanced Features](wiki/Advanced-Features.md) |
| **API Reference** | Complete API documentation | [API Reference](wiki/API-Reference.md) |
| **Examples** | Real-world usage patterns | [Examples](wiki/Examples.md) |
| **Troubleshooting** | Common issues and solutions | [Troubleshooting](wiki/Troubleshooting.md) |
| **FAQ** | Frequently asked questions | [FAQ](wiki/FAQ.md) |

## Implementation Status: 100% COMPLETE

### **Phase 1 Complete - Cryptographic Foundation** ✅
- **AEAD Cryptography**: Production-ready ChaCha20-Poly1305 authenticated encryption via libsodium
- **Production Hybrid Key Exchange**: **Real X25519 + ML-KEM768** implementation using liboqs
- **Post-Quantum Security**: Genuine quantum-resistant cryptography
- **Base64 Codec**: Complete encoder/decoder with HTML entities support
- **DNS Packet Builder**: Structured packet construction with random IDs and headers
- **UDP Socket Communication**: DNS query transmission with timeout support

### **Phase 2 Complete - Transport Layer Development** ✅
- **DNS Response Parsing**: Complete bidirectional communication with domain decompression
- **Transport Abstraction**: `ITransport` interface for pluggable protocols
- **DoH (DNS-over-HTTPS)**: Production HTTPS transport with libcurl
- **DoT (DNS-over-TLS)**: Production TLS transport with OpenSSL
- **Behavioral Mimicry**: Traffic timing profiles and evasion techniques
- **Adaptive Transport Manager**: Dynamic transport switching for detection evasion
- **Async I/O Framework**: High-performance asynchronous I/O with kqueue/epoll support

### **Phase 3 Complete - Steganographic Enhancement** ✅
- **Multi-record DNS Encoding**: A, AAAA, and enhanced TXT record support
- **IPv4/IPv6 Address Encoding**: Steganographic data embedding in IP addresses
- **HTTP/2 Body Encoding**: DoH transport with steganographic HTTP/2 bodies
- **Advanced Payload Distribution**: Intelligent fragment distribution and noise injection
- **Response Parsing and Extraction**: Bidirectional steganographic communication
- **Compression Support**: Payload compression using zlib for increased capacity
- **Fragment Management**: Randomization, noise injection, and integrity verification
- **Capacity Estimation**: Dynamic payload capacity calculation for optimal encoding

## Technical Stack

- **Language**: C++20
- **Build**: CMake 3.16+
- **Cryptography**: libsodium (AEAD + X25519), liboqs (ML-KEM768)
- **Transport**: libcurl (DoH), OpenSSL (DoT), UDP sockets
- **Compression**: zlib for payload optimization
- **Platform**: Cross-platform (Linux, macOS, Windows)

## Performance Metrics

Based on comprehensive testing:

- **Encoding Performance**: ~2.2ms for 100 operations
- **Transport Creation**: ~462μs for 100 instances
- **Capacity Estimation**: ~2μs for 1000 operations
- **Estimated Capacity**: ~220 bytes per message
- **Compression**: Variable ratio based on payload content
- **Fragment Distribution**: Intelligent multi-record strategy

## Test Coverage

**Unified Test Suite: 15/15 tests passing across all phases:**

### Core Functionality Tests (4/4) ✅
- Base64 encoding/decoding with edge cases
- AEAD cryptography (ChaCha20-Poly1305)
- Hybrid key exchange (X25519 + ML-KEM768)
- DNS packet construction with multiple record types

### Transport Layer Tests (3/3) ✅
- Transport abstraction (UDP, DoH, DoT)
- Behavioral mimicry with timing variance
- Async I/O framework with concurrent operations

### Steganographic Enhancement Tests (6/6) ✅
- Multi-record DNS encoding strategies
- IPv4/IPv6 address encoding capabilities
- Enhanced TXT record encoding
- HTTP/2 body encoding for DoH
- Capacity estimation algorithms
- Fragment management with noise injection

### Integration & Performance Tests (2/2) ✅
- End-to-end steganographic flow
- Performance benchmarks and optimization

## Test Suite Usage

```bash
# Run all tests (recommended)
./chimera_test --all

# Run specific test categories
./chimera_test --core              # Core functionality only
./chimera_test --transport         # Transport layer only
./chimera_test --steganography     # Phase 3 features only
./chimera_test --integration       # Integration tests only
./chimera_test --performance       # Performance benchmarks only

# Quick essential tests
./chimera_test --quick

# Verbose output
./chimera_test --verbose

# Help and usage
./chimera_test --help
```

## Development Roadmap

### **Phase 1: Cryptographic Foundation** ✅ **COMPLETE**
### **Phase 2: Transport Layer** ✅ **COMPLETE**
### **Phase 3: Steganographic Enhancement** ✅ **COMPLETE**

### **Future Enhancements** (Optional)
- [ ] Machine learning-based detection evasion
- [ ] Advanced traffic analysis resistance
- [ ] GUI interface for non-technical users
- [ ] Additional transport protocols (QUIC, HTTP/3)
- [ ] Mobile platform support (Android/iOS)

## Usage Examples

### Basic Message Transmission
```bash
# Simple message
./chimera_demo "Hello World"

# Multi-record encoding with compression
./chimera_demo --encoding multi-record --compress "Large message content"

# Custom DNS server and domain
./chimera_demo --server 1.1.1.1 --domain custom.example.com "Custom setup"
```

### Advanced Configuration
```cpp
chimera::ClientConfig config;
config.dns_server = "8.8.8.8";
config.target_domain = "example.com";
config.transport = chimera::TransportType::DoH;
config.encoding_strategy = chimera::EncodingStrategy::MULTI_RECORD;
config.use_compression = true;
config.noise_ratio = 0.15;
config.max_fragments = 5;
config.timing_variance = std::chrono::milliseconds(100);

chimera::ChimeraClient client(config);
```

## Contributing

**All phases completed successfully!** Contributions welcome for:
- Performance optimizations
- Additional transport protocols
- Enhanced evasion techniques
- Documentation improvements
- Platform-specific optimizations

For detailed contribution guidelines and development setup, see: **[Contributing Guide](wiki/Contributing.md)**

## Security Considerations

CHIMERA implements production-grade security:
- **Post-Quantum Protection**: Real ML-KEM768 via liboqs
- **Classical Encryption**: Production-grade ChaCha20-Poly1305 via libsodium
- **Hybrid Security**: Combined classical + quantum resistance
- **Memory Safety**: Modern C++ practices with RAII and bounds checking
- **Transport Security**: TLS 1.3 for DoH/DoT channels
- **Steganographic Security**: Multi-layer obfuscation with noise injection

For complete security documentation, see: **[Security Guide](wiki/Security.md)**

## License

Apache License 2.0 - See LICENSE file for details.

## Acknowledgments

Special thanks to:
- **libsodium project** for production-grade cryptography
- **Open Quantum Safe (liboqs)** for post-quantum cryptography
- **The cryptography research community** for foundational work
- **Contributors and testers** who helped validate the implementation

---

**📖 Complete Documentation**: Visit our **[Wiki](wiki/Home.md)** for comprehensive guides, examples, and API reference.

**Current Status**: All three phases complete - CHIMERA is production-ready!  
**Milestone**: Full steganographic framework with quantum-resistant security, multi-transport support, and advanced payload distribution.