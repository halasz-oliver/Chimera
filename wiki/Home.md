# CHIMERA Framework Wiki

Welcome to the **CHIMERA** (Covert Hybrid Intelligence via Masked Encrypted Radio Applications) framework documentation. CHIMERA is a steganographic communication system that uses DNS queries to transmit encrypted messages covertly.

## Quick Navigation

### Getting Started
- **[Getting Started](Getting-Started.md)** - Installation, dependencies, and first steps
- **[Basic Usage](Basic-Usage.md)** - Simple examples to get you started
- **[Configuration](Configuration.md)** - Complete configuration reference

### Core Documentation
- **[API Reference](API-Reference.md)** - Complete API documentation
- **[Examples](Examples.md)** - Code examples and use cases

### Support
- **[Troubleshooting](Troubleshooting.md)** - Common issues and solutions
- **[FAQ](FAQ.md)** - Frequently asked questions

## Overview

CHIMERA provides a steganographic communication framework with the following implemented features:

### Phase 1 - Cryptographic Foundation ✓ COMPLETE
- **Quantum-Resistant Security**: X25519 + ML-KEM768 hybrid key exchange
- **AEAD Encryption**: ChaCha20-Poly1305 authenticated encryption
- **DNS Steganography**: Covert message transmission via DNS TXT records
- **Production-Ready**: Full error handling, timeouts, and validation

### Phase 2 - Transport Layer ✓ COMPLETE  
- **Multiple Transports**: UDP/53, DNS-over-HTTPS (DoH), DNS-over-TLS (DoT)
- **Behavioral Mimicry**: Traffic patterns and timing variance
- **Async I/O Framework**: Asynchronous operations support

## Architecture

```
┌─────────────────┐    
│   Application   │    
├─────────────────┤    
│ CHIMERA Client  │    
├─────────────────┤    
│ Transport Layer │    
│ (UDP/DoH/DoT)   │    
├─────────────────┤    
│ DNS Packets     │    
└─────────────────┘    
         │              
         │              
    ┌─────────────────┐
    │   DNS Server    │
    │  (example.com)  │
    └─────────────────┘
```

## Quick Start

```cpp
#include "chimera/client.hpp"

// Basic configuration
chimera::ClientConfig config;
config.dns_server = "8.8.8.8";           // Google's DNS
config.target_domain = "example.com";    // Your target domain

// Create client
chimera::ChimeraClient client(config);

// Send a message
auto result = client.send_text("Hello, CHIMERA!");

if (result) {
    std::cout << "Message sent successfully!" << std::endl;
    std::cout << "Bytes sent: " << result->bytes_sent << std::endl;
    std::cout << "Latency: " << result->latency.count() << "ms" << std::endl;
    std::cout << "Domain used: " << result->used_domain << std::endl;
} else {
    std::cerr << "Failed to send message" << std::endl;
}
```

## Key Benefits

1. **Covert Communication**: Messages hidden in legitimate-looking DNS traffic
2. **Quantum-Resistant**: Future-proof cryptography (ML-KEM768 + X25519)
3. **Multi-Transport**: Supports UDP, HTTPS, and TLS for different security requirements
4. **Behavioral Evasion**: Traffic patterns and timing variance
5. **Production Ready**: Comprehensive error handling, logging, and testing

## System Requirements

- **C++17** or later
- **CMake 3.15+**
- **Dependencies**: libsodium, liboqs, libcurl, OpenSSL
- **Platforms**: macOS, Linux, Windows (with appropriate compilers)

## License

CHIMERA is released under the MIT License. See the main repository for license details.

---

**Next Step**: Start with [Getting Started](Getting-Started.md) to install and configure CHIMERA.