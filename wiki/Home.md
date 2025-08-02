# CHIMERA Framework Wiki

Welcome to the **CHIMERA** (Covert Hybrid Intelligence via Masked Encrypted Radio Applications) framework documentation. CHIMERA is a production-ready steganographic communication system that uses DNS queries to transmit encrypted messages covertly.

## Quick Navigation

### Getting Started
- **[Getting Started](Getting-Started.md)** - Installation, dependencies, and first steps
- **[Basic Usage](Basic-Usage.md)** - Simple examples to get you started
- **[Configuration](Configuration.md)** - Complete configuration reference

### Core Documentation
- **[API Reference](API-Reference.md)** - Complete API documentation
- **[Examples](Examples.md)** - Code examples and use cases
- **[Advanced Features](Advanced-Features.md)** - Phase 2 and 3 advanced capabilities

### Support
- **[Troubleshooting](Troubleshooting.md)** - Common issues and solutions
- **[FAQ](FAQ.md)** - Frequently asked questions

## Overview

CHIMERA provides a comprehensive steganographic communication framework with all phases fully implemented and production-ready:

### Phase 1 - Cryptographic Foundation ✅ COMPLETE
- **Quantum-Resistant Security**: X25519 + ML-KEM768 hybrid key exchange
- **AEAD Encryption**: ChaCha20-Poly1305 authenticated encryption
- **DNS Steganography**: Covert message transmission via DNS records
- **Production-Ready**: Full error handling, timeouts, and validation

### Phase 2 - Transport Layer ✅ COMPLETE  
- **Multiple Transports**: UDP/53, DNS-over-HTTPS (DoH), DNS-over-TLS (DoT)
- **Behavioral Mimicry**: Traffic patterns and timing variance
- **Async I/O Framework**: High-performance asynchronous operations
- **Transport Switching**: Dynamic protocol selection for evasion

### Phase 3 - Steganographic Enhancement ✅ COMPLETE
- **Multi-record DNS Encoding**: A, AAAA, and enhanced TXT record support
- **IPv4/IPv6 Address Encoding**: Data embedding in IP addresses
- **HTTP/2 Body Encoding**: Steganographic HTTP/2 request bodies for DoH
- **Advanced Payload Distribution**: Intelligent fragmentation and noise injection
- **Compression Support**: Payload optimization using zlib
- **Capacity Estimation**: Dynamic capacity calculation for optimal encoding

## Architecture

```
┌─────────────────────────────────────────┐    
│              Application                │    
├─────────────────────────────────────────┤    
│            CHIMERA Client               │    
│  ┌─────────────┬─────────────────────┐  │
│  │ Crypto      │ Steganographic      │  │
│  │ (Phase 1)   │ Enhancement         │  │
│  │             │ (Phase 3)           │  │
│  └─────────────┴─────────────────────┘  │
├─────────────────────────────────────────┤    
│         Transport Layer (Phase 2)       │    
│    ┌─────────┬─────────┬─────────┐     │
│    │   UDP   │   DoH   │   DoT   │     │
│    │  :53    │ :443    │  :853   │     │
│    └─────────┴─────────┴─────────┘     │
├─────────────────────────────────────────┤    
│      Multi-record DNS Packets          │    
│   ┌───────┬─────────┬─────────────┐    │
│   │   A   │  AAAA   │     TXT     │    │
│   │ IPv4  │  IPv6   │  Enhanced   │    │
│   └───────┴─────────┴─────────────┘    │
└─────────────────────────────────────────┘    
                     │                          
                     │                          
        ┌─────────────────────────────┐
        │        DNS Server           │
        │     (example.com)           │
        │                             │
        │  Steganographic Responses   │
        └─────────────────────────────┘
```

## Quick Start

```cpp
#include "chimera/client.hpp"

// Phase 3 configuration with all features
chimera::ClientConfig config;
config.dns_server = "8.8.8.8";           // Google's DNS
config.target_domain = "example.com";    // Your target domain
config.transport = chimera::TransportType::DoH;  // HTTPS transport
config.encoding_strategy = chimera::EncodingStrategy::MULTI_RECORD;
config.use_compression = true;           // Enable payload compression
config.noise_ratio = 0.1;               // 10% noise injection
config.max_fragments = 5;               // Maximum fragments per message

// Create client
chimera::ChimeraClient client(config);

// Send a message
auto result = client.send_text("Hello, CHIMERA!");

if (result) {
    std::cout << "Message sent successfully!" << std::endl;
    std::cout << "Fragments sent: " << result->fragments_sent << std::endl;
    std::cout << "Compression used: " << (result->compression_used ? "yes" : "no") << std::endl;
    std::cout << "Record types: " << result->used_record_types.size() << std::endl;
    std::cout << "Latency: " << result->latency.count() << "ms" << std::endl;
} else {
    std::cerr << "Failed to send message" << std::endl;
}
```

## Key Features

### Security Features
1. **Post-Quantum Cryptography**: ML-KEM768 + X25519 hybrid key exchange
2. **Authenticated Encryption**: ChaCha20-Poly1305 AEAD
3. **Transport Security**: TLS 1.3 for DoH/DoT channels
4. **Memory Safety**: Modern C++ with RAII and bounds checking

### Steganographic Features
1. **Multi-record Encoding**: Data distributed across A, AAAA, and TXT records
2. **IPv4/IPv6 Embedding**: Steganographic data in IP addresses
3. **HTTP/2 Body Encoding**: Custom HTTP/2 request bodies for DoH
4. **Compression**: Payload optimization for increased capacity
5. **Noise Injection**: Randomized fragments for detection evasion

### Performance Features
1. **Async I/O**: High-performance asynchronous operations
2. **Transport Switching**: Dynamic protocol selection
3. **Behavioral Mimicry**: Traffic pattern variance
4. **Capacity Optimization**: Intelligent payload distribution

## Test Coverage

CHIMERA includes comprehensive testing with **15/15 tests passing**:

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

## System Requirements

- **C++20** or later
- **CMake 3.16+**
- **Dependencies**: libsodium, liboqs, libcurl, OpenSSL, zlib
- **Platforms**: macOS, Linux, Windows (with appropriate compilers)

## Performance Metrics

Based on comprehensive testing:
- **Encoding Performance**: ~2.2ms for 100 operations
- **Transport Creation**: ~462μs for 100 instances
- **Capacity Estimation**: ~2μs for 1000 operations
- **Estimated Capacity**: ~220 bytes per message
- **Typical DNS Latency**: Variable by server (usually 20-100ms)

## License

CHIMERA is released under the Apache License 2.0. See the main repository for license details.

---

**Next Step**: Start with [Getting Started](Getting-Started.md) to install and configure CHIMERA, or jump to [Basic Usage](Basic-Usage.md) for immediate examples.