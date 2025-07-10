# CHIMERA - Post-Quantum Polymorphic Steganography

**Quantum-Safe, Adaptive Covert-Channel Framework**

A polymorphic framework designed to create practically undetectable, hidden communication channels by disguising data as benign network traffic with post-quantum cryptographic security.

## Project Status

**PHASE 2 IN PROGRESS!** Transport layer foundation complete with DNS response parsing and pluggable transport architecture. Phase 1 cryptographic core remains production-ready.

This is an ongoing learning and development project. For feedback, contributions, or collaboration, email me at `halaszoliver45(at)gmail.com`.

## Project Goals

CHIMERA aims to be a comprehensive steganographic communication system that:
- **Embeds data in DNS traffic** (DoH/DoT channels) to appear as standard encrypted HTTPS traffic
- **Implements post-quantum cryptography** with hybrid key exchange (X25519+ML-KEM768)
- **Dynamically switches between transport layers** (DoH, DoT, UDP/53) using behavioral timing profiles to evade detection
- Targets **3 Mbps throughput** with false positive rate under 1% on intrusion detection systems

## Current Implementation Status

### **Phase 1 Complete - Production Cryptographic Foundation**

- ✅ **AEAD Cryptography**: Production-ready ChaCha20-Poly1305 authenticated encryption via libsodium
- ✅ **Production Hybrid Key Exchange**: **Real X25519 + ML-KEM768** implementation using liboqs
- ✅ **Post-Quantum Security**: Genuine quantum-resistant cryptography (no more placeholders!)
- ✅ **Base64 Codec**: Complete encoder/decoder with HTML entities fixed
- ✅ **DNS Packet Builder**: Structured packet construction with random IDs and headers
- ✅ **UDP Socket Communication**: DNS query transmission with timeout support
- ✅ **Client Configuration**: Flexible config system with validation
- ✅ **CLI Interface**: Command-line argument parsing and file input support
- ✅ **Build System**: Cross-platform CMake with liboqs integration
- ✅ **Random Subdomain Generation**: Dynamic domain generation for steganographic queries
- ✅ **Production Test Suite**: Comprehensive testing with real post-quantum crypto validation
- ✅ **HKDF Key Derivation**: Secure key derivation from hybrid secrets

### **Phase 2 In Progress - Transport Layer Development**

**Recently Completed:**
- ✅ **DNS Response Parsing**: Complete bidirectional communication with domain decompression
- ✅ **Transport Abstraction**: `ITransport` interface for pluggable protocols
- ✅ **UDP Transport**: `TransportUdp` implementation with error handling
- ✅ **Enhanced Client**: Updated to use transport abstraction instead of raw sockets
- ✅ **Improved Ping**: Actual DNS response parsing and validation
- ✅ **Security Hardening**: Pointer jump protection and bounds checking

**Next Priorities:**
- 🔄 **TLS/DoH/DoT Support**: Secure transport layer implementation
- 🔄 **Behavioral Mimicry**: Traffic timing profiles and evasion techniques
- 🔄 **Async I/O**: High-performance io_uring integration

### **Current Security Status**

- **Post-Quantum Security**: Real ML-KEM768 via NIST-standardized liboqs
- **Classical Security**: Production X25519 + ChaCha20-Poly1305 via libsodium
- **Hybrid Protection**: Defense against both current and quantum attacks
- **Forward Secrecy**: Ephemeral keys per session
- **Authentication**: AEAD prevents tampering and forgery
- **Memory Safety**: Bounds checking and pointer jump protection in DNS parsing
- **Production Ready**: No placeholders remaining in crypto stack

## Getting Started

### Prerequisites

1. **C++20 Compiler**: GCC 12+ or Clang 15+
2. **CMake**: Version 3.16+
3. **libsodium**: Core cryptography library
4. **liboqs**: Post-quantum cryptography library (**Required for Phase 1**)
5. **tl::expected**: Header-only error handling (included)

### Installing Dependencies

**Ubuntu/Debian:**
```
sudo apt-get update && sudo apt-get install -y libsodium-dev build-essential cmake

# Install liboqs
sudo apt-get install -y liboqs-dev
# If not available, build from source:
git clone https://github.com/open-quantum-safe/liboqs.git
cd liboqs && mkdir build && cd build
cmake .. -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release
make -j$(nproc) && sudo make install
sudo ldconfig
```

**macOS (Homebrew):**
```
brew install libsodium liboqs cmake
```

**Arch Linux:**
```
sudo pacman -S libsodium cmake gcc
# For liboqs, use AUR or build from source
```

### Building

```
# Clone repository
git clone https://github.com/your-username/chimera.git
cd chimera

# Configure and build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Usage

### Basic Demo
```
# Send default message
./build/chimera_demo

# Send file contents
./build/chimera_demo message.txt

# Custom DNS server and domain
./build/chimera_demo -s 1.1.1.1 -d covert.example.com message.txt
```

### Command Options
- `-s, --server`: DNS server (default: 8.8.8.8)
- `-p, --port`: DNS port (default: 53)
- `-d, --domain`: Target domain (default: example.com)
- `-t, --timeout`: Timeout in ms (default: 5000)
- `-h, --help`: Show usage

### Testing
```
# Run comprehensive test suite (includes REAL post-quantum crypto tests!)
./build/chimera_test

# Or via make target
make run_tests
```

## Architecture

### Core Components

| Component | Status | Description |
|-----------|--------|-------------|
| **AEAD Module** | Production | ChaCha20-Poly1305 encryption via libsodium |
| **Hybrid KEM** | Production | **Real X25519 + ML-KEM768** via liboqs |
| **DNS Codec** | Complete | Packet building with TXT record embedding |
| **DNS Parser** | **Complete** | **Response parsing with domain decompression** |
| **Transport Layer** | **Complete** | **Pluggable transport abstraction** |
| **Client Interface** | Enhanced | High-level steganographic communication API |
| **Base64 Codec** | Production | Fixed HTML entities, proper padding |

### **Phase 2 Transport Architecture**

CHIMERA now implements **production-grade** transport abstraction:

1. **Transport Interface** (`ITransport`):
   - Pluggable protocol support (UDP, DoH, DoT)
   - Standardized error handling with `TransportError`
   - Timeout management and configuration

2. **UDP Transport** (`TransportUdp`):
   - Complete UDP/53 implementation
   - Socket management with RAII
   - Comprehensive error handling

3. **DNS Response Parsing**:
   - Full domain name decompression
   - Pointer jump protection (security)
   - Resource record extraction
   - Bounds checking for memory safety

**Architecture Benefits**: The transport abstraction enables rapid development of DoH and DoT protocols without changing client code, while maintaining the production-ready security from Phase 1.

## Technical Stack

- **Language**: C++20
- **Build**: CMake 3.16+
- **Cryptography**:
  - **libsodium 1.0.19+**: Production AEAD + X25519
  - **liboqs 0.8.0+**: Production ML-KEM768 (Kyber-768)
  - **HKDF**: Key derivation from hybrid secrets
- **Transport**: Pluggable interface with UDP implementation
- **Error Handling**: tl::expected functional error management
- **Testing**: Custom test framework with real crypto validation
- **Platform**: Cross-platform (Linux, macOS, Windows)

## Development Roadmap

### **Phase 1: Cryptographic Foundation** ✅ **COMPLETE**
- [x] Production libsodium AEAD integration
- [x] **Real X25519 + ML-KEM768** hybrid key exchange via liboqs
- [x] HKDF key derivation
- [x] HTML entity fixes in base64.hpp
- [x] Comprehensive cryptographic testing
- [x] **ALL PHASE 1 ITEMS COMPLETE**

### **Phase 2: Transport Layer** 🔄 **IN PROGRESS**
- [x] Abstract transport interfaces (ITransport)
- [x] UDP transport implementation with error handling
- [x] DNS response parsing with domain decompression
- [x] Bidirectional communication foundation
- [ ] TLS 1.3 integration for DoH/DoT
- [ ] High-performance I/O with io_uring
- [ ] HTTP/2 DoH implementation
- [ ] DNS-over-TLS (DoT) implementation

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

**Phase 2 development is underway!** Contributions are welcome:

1. **Current Focus Areas:**
   - TLS/DoH/DoT transport implementations
   - Async I/O integration
   - Performance optimization

2. **How to Contribute:**
   - Submit pull requests with clear descriptions
   - Open issues for architectural discussions
   - Email for mentoring or collaboration

3. **Learning Opportunities:**
   - High-performance networking
   - TLS 1.3 implementation
   - Traffic analysis evasion

## Security Considerations

### **Production Security Status**
- **Post-Quantum Protection**: Real ML-KEM768 via liboqs
- **Classical Encryption**: Production-grade via libsodium
- **Hybrid Security**: Combined classical + quantum resistance
- **Memory Safety**: Modern C++ practices with RAII and bounds checking
- **Secure Randomness**: Cryptographically secure RNG
- **Side-Channel Resistance**: Inherited from libsodium/liboqs
- **DNS Security**: Pointer jump protection and compression validation

### **Cryptographic Assurance**
- **No Placeholders**: All cryptographic components are production-ready
- **NIST Standards**: ML-KEM768 follows NIST post-quantum standards
- **Proven Libraries**: Built on battle-tested libsodium and liboqs
- **Constant-Time Operations**: Side-channel resistant implementations

## License

Apache License 2.0 - See LICENSE file for details.

## Acknowledgments

Special thanks to:
- **libsodium project** for robust cryptographic primitives
- **Open Quantum Safe (liboqs)** for production post-quantum cryptography
- **tl::expected** for functional error handling
- The cryptography and steganography research community

**Current Milestone**: Phase 2 transport foundation complete. The project now provides pluggable transport abstraction with production DNS response parsing, maintaining quantum-resistant security while enabling rapid protocol development.

**Next**: Completing TLS integration for DoH/DoT protocols, building on the solid cryptographic and transport foundations.