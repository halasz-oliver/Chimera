# CHIMERA - Post-Quantum Polymorphic Steganography

**Quantum-Safe, Adaptive Covert-Channel Framework**

A polymorphic framework designed to create practically undetectable, hidden communication channels by disguising data as benign network traffic with post-quantum cryptographic security.

## Project Status

**PHASE 2 COMPLETE!** Full transport layer implementation with DoH/DoT support, behavioral mimicry, and high-performance async I/O. Phase 1 cryptographic core remains production-ready.

This is an ongoing learning and development project. For feedback, contributions, or collaboration, email me at `halaszoliver45(at)gmail.com`.

## Project Goals

CHIMERA aims to be a comprehensive steganographic communication system that:
- **Embeds data in DNS traffic** (DoH/DoT channels) to appear as standard encrypted HTTPS traffic
- **Implements post-quantum cryptography** with hybrid key exchange (X25519+ML-KEM768)
- **Dynamically switches between transport layers** (DoH, DoT, UDP/53) using behavioral timing profiles to evade detection
- Targets **3 Mbps throughput** with false positive rate under 1% on intrusion detection systems

## 🚀 Quick Start

For complete setup instructions, detailed usage examples, and configuration options, please visit our **[Wiki](wiki/Home.md)**.

### Essential Setup
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt install libsodium-dev liboqs-dev cmake build-essential

# Build
mkdir build && cd build && cmake .. && make -j$(nproc)

# Test
./chimera_demo "Hello, CHIMERA!"
```

**For detailed installation instructions for all platforms, see:** **[Getting Started Guide](wiki/Getting-Started.md)**

### Basic Usage
```cpp
#include "chimera/client.hpp"

chimera::ClientConfig config;
config.dns_server = "8.8.8.8";
config.target_domain = "example.com";

chimera::ChimeraClient client(config);
auto result = client.send_text("Secret message");
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

## Current Implementation Status

### **Phase 1 Complete - Production Cryptographic Foundation**
- **AEAD Cryptography**: Production-ready ChaCha20-Poly1305 authenticated encryption via libsodium
- **Production Hybrid Key Exchange**: **Real X25519 + ML-KEM768** implementation using liboqs
- **Post-Quantum Security**: Genuine quantum-resistant cryptography (no more placeholders!)
- **Base64 Codec**: Complete encoder/decoder with HTML entities fixed
- **DNS Packet Builder**: Structured packet construction with random IDs and headers
- **UDP Socket Communication**: DNS query transmission with timeout support

### **Phase 2 Complete - Transport Layer Development**
- **DNS Response Parsing**: Complete bidirectional communication with domain decompression
- **Transport Abstraction**: `ITransport` interface for pluggable protocols
- **DoH (DNS-over-HTTPS)**: Production HTTPS transport with libcurl
- **DoT (DNS-over-TLS)**: Production TLS transport with OpenSSL
- **Behavioral Mimicry**: Traffic timing profiles and evasion techniques
- **Adaptive Transport Manager**: Dynamic transport switching for detection evasion
- **Async I/O Framework**: High-performance asynchronous I/O with kqueue/epoll support

## Technical Stack

- **Language**: C++20
- **Build**: CMake 3.16+
- **Cryptography**: libsodium (AEAD + X25519), liboqs (ML-KEM768)
- **Transport**: libcurl (DoH), OpenSSL (DoT), UDP
- **Platform**: Cross-platform (Linux, macOS, Windows)

## Development Roadmap

### **Phase 1: Cryptographic Foundation** ✅ **COMPLETE**
### **Phase 2: Transport Layer** ✅ **COMPLETE**

### **Phase 3: Steganographic Enhancement**
- [ ] Multi-record DNS encoding (A, AAAA, TXT)
- [ ] HTTP/2 body encoding for DoH
- [ ] Advanced payload distribution
- [ ] Response parsing and extraction

### **Phase 4: Behavioral Mimicry**
- [ ] Traffic timing profiles
- [ ] Detection evasion techniques
- [ ] Adaptive transport switching
- [ ] Behavioral pattern configuration

### **Phase 5: Production Readiness**
- [ ] Security audit
- [ ] Performance optimization
- [ ] Documentation completion
- [ ] Integration testing

## Contributing

**Phase 2 completed successfully!** Contributions welcome for Phase 3.

For detailed contribution guidelines and development setup, see: **[Contributing Guide](wiki/Contributing.md)**

## Security Considerations

CHIMERA implements production-grade security:
- **Post-Quantum Protection**: Real ML-KEM768 via liboqs
- **Classical Encryption**: Production-grade via libsodium
- **Hybrid Security**: Combined classical + quantum resistance
- **Memory Safety**: Modern C++ practices with RAII and bounds checking

For complete security documentation, see: **[Security Guide](wiki/Security.md)**

## License

Apache License 2.0 - See LICENSE file for details.

## Acknowledgments

Special thanks to the **libsodium project**, **Open Quantum Safe (liboqs)**, and the cryptography research community.

---

**📖 Complete Documentation**: Visit our **[Wiki](wiki/Home.md)** for comprehensive guides, examples, and API reference.

**Current Milestone**: Phase 2 transport layer complete with full DoH/DoT implementation, behavioral mimicry, and async I/O.  
**Next**: Phase 3 steganographic enhancements.