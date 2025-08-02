# API Reference

This document provides a complete reference for the CHIMERA API, covering all three phases of implementation.

## Table of Contents

- [Core Classes](#core-classes)
- [Configuration](#configuration)
- [Enumerations](#enumerations)
- [Result Types](#result-types)
- [Error Handling](#error-handling)
- [Examples](#examples)

## Core Classes

### ChimeraClient

The main client class for CHIMERA operations.

```cpp
class ChimeraClient {
public:
    explicit ChimeraClient(const ClientConfig& config);
    
    // Text message operations
    tl::expected<SendResult, ChimeraError> send_text(const std::string& message) const;
    
    // Binary data operations
    tl::expected<SendResult, ChimeraError> send_data(const std::vector<uint8_t>& data) const;
    
    // File operations
    tl::expected<SendResult, ChimeraError> send_file(const std::string& file_path) const;
    
    // Multi-record encoding (Phase 3)
    tl::expected<SendResult, ChimeraError> send_multi_record(const std::vector<uint8_t>& data) const;
    
    // Receive operations
    tl::expected<std::vector<uint8_t>, ChimeraError> receive_data(const std::string& query_domain) const;
    
    // Capacity estimation (Phase 3)
    size_t estimate_capacity() const;
    
    // Network testing
    tl::expected<std::chrono::milliseconds, ChimeraError> ping_dns_server() const;
    
    // Configuration access
    const ClientConfig& get_config() const;
    void update_config(ClientConfig new_config);
};
```

### ClientConfig

Configuration structure for CHIMERA client.

```cpp
struct ClientConfig {
    // Basic DNS settings
    std::string dns_server = "8.8.8.8";
    uint16_t dns_port = 53;
    std::string target_domain = "example.com";
    std::chrono::milliseconds timeout{5000};
    
    // Cryptographic settings
    bool use_random_subdomains = true;
    bool use_hybrid_crypto = true;  // Enable hybrid key exchange
    
    // Transport settings (Phase 2)
    TransportType transport = TransportType::UDP;
    bool adaptive_transport = false;  // Behavioral mimicry
    std::chrono::milliseconds timing_variance{100};  // Jitter for behavioral mimicry
    BehavioralProfile behavioral_profile = BehavioralProfile::Normal;
    
    // Steganographic settings (Phase 3)
    EncodingStrategy encoding_strategy = EncodingStrategy::MULTI_RECORD;
    bool use_compression = true;
    bool randomize_fragments = true;
    double noise_ratio = 0.1;
    size_t max_fragments = 10;
};
```

## Configuration

### Basic Configuration

```cpp
chimera::ClientConfig config;
config.dns_server = "1.1.1.1";           // Cloudflare DNS
config.target_domain = "example.com";    // Target domain
config.timeout = std::chrono::milliseconds(3000);  // 3 second timeout
```

### Transport Configuration (Phase 2)

```cpp
// DNS-over-HTTPS
config.transport = chimera::TransportType::DoH;

// DNS-over-TLS
config.transport = chimera::TransportType::DoT;

// UDP (default)
config.transport = chimera::TransportType::UDP;
config.dns_port = 53;
```

### Steganographic Configuration (Phase 3)

```cpp
// Multi-record encoding
config.encoding_strategy = chimera::EncodingStrategy::MULTI_RECORD;

// Enable compression
config.use_compression = true;

// Add noise for evasion (10% noise)
config.noise_ratio = 0.1;

// Limit fragments
config.max_fragments = 5;
```

### Behavioral Configuration (Phase 2)

```cpp
// Enable behavioral mimicry
config.timing_variance = std::chrono::milliseconds(200);
config.adaptive_transport = true;
config.behavioral_profile = chimera::BehavioralProfile::WebBrowsing;
```

## Enumerations

### TransportType

```cpp
enum class TransportType {
    UDP,    // Standard UDP DNS (port 53)
    DoH,    // DNS-over-HTTPS (port 443)
    DoT     // DNS-over-TLS (port 853)
};
```

### EncodingStrategy

```cpp
enum class EncodingStrategy {
    TXT_ONLY,       // Traditional TXT record encoding
    MULTI_RECORD,   // Phase 3: A, AAAA, and TXT records
    DISTRIBUTED,    // Phase 3: Advanced payload distribution
    HTTP2_BODY      // Phase 3: HTTP/2 body encoding for DoH
};
```

### BehavioralProfile

```cpp
enum class BehavioralProfile {
    Normal,        // Regular DNS queries
    WebBrowsing,   // Web browsing patterns
    Enterprise,    // Corporate network patterns
    Gaming,        // Gaming traffic patterns
    Random         // Random pattern selection
};
```

### ChimeraError

```cpp
enum class ChimeraError {
    NetworkError,       // Network communication failed
    ConfigError,        // Invalid configuration
    EncodingError,      // Data encoding/decoding failed
    DecodingError,      // Data decoding failed
    TimeoutError,       // Operation timed out
    DnsError,           // DNS-specific error
    CryptoError         // Cryptographic operation failed
};
```

## Result Types

### SendResult

```cpp
struct SendResult {
    size_t bytes_sent;                    // Total bytes transmitted
    std::chrono::milliseconds latency;    // Round-trip time
    std::string used_domain;              // Domain actually used
    
    // Phase 3 fields
    std::vector<DnsType> used_record_types;  // DNS record types used
    size_t fragments_sent;                   // Number of fragments sent
    EncodingStrategy encoding_used;          // Encoding strategy used
    bool compression_used;                   // Whether compression was applied
};
```

## Error Handling

CHIMERA uses `tl::expected` for error handling:

```cpp
auto result = client.send_text("Hello, World!");

if (result) {
    // Success
    std::cout << "Sent " << result->bytes_sent << " bytes" << std::endl;
    std::cout << "Latency: " << result->latency.count() << "ms" << std::endl;
    
    // Phase 3 information
    if (result->fragments_sent > 0) {
        std::cout << "Fragments: " << result->fragments_sent << std::endl;
        std::cout << "Record types used: " << result->used_record_types.size() << std::endl;
        std::cout << "Compression: " << (result->compression_used ? "yes" : "no") << std::endl;
    }
} else {
    // Error
    switch (result.error()) {
        case chimera::ChimeraError::NetworkError:
            std::cerr << "Network error occurred" << std::endl;
            break;
        case chimera::ChimeraError::TimeoutError:
            std::cerr << "Operation timed out" << std::endl;
            break;
        case chimera::ChimeraError::CryptoError:
            std::cerr << "Cryptographic error" << std::endl;
            break;
        // ... handle other errors
    }
}
```

## Examples

### Basic Usage

```cpp
#include "chimera/client.hpp"

int main() {
    // Basic configuration
    chimera::ClientConfig config;
    config.dns_server = "8.8.8.8";
    config.target_domain = "example.com";
    
    // Create client
    chimera::ChimeraClient client(config);
    
    // Send message
    auto result = client.send_text("Hello, CHIMERA!");
    
    if (result) {
        std::cout << "Message sent successfully!" << std::endl;
    } else {
        std::cerr << "Failed to send message" << std::endl;
    }
    
    return 0;
}
```

### Advanced Phase 3 Usage

```cpp
#include "chimera/client.hpp"

int main() {
    // Advanced Phase 3 configuration
    chimera::ClientConfig config;
    config.dns_server = "1.1.1.1";
    config.target_domain = "example.com";
    config.transport = chimera::TransportType::DoH;
    config.encoding_strategy = chimera::EncodingStrategy::MULTI_RECORD;
    config.use_compression = true;
    config.noise_ratio = 0.15;  // 15% noise
    config.max_fragments = 3;
    config.timing_variance = std::chrono::milliseconds(150);
    
    chimera::ChimeraClient client(config);
    
    // Estimate capacity
    size_t capacity = client.estimate_capacity();
    std::cout << "Estimated capacity: " << capacity << " bytes" << std::endl;
    
    // Send large message with compression
    std::string large_message = "This is a large message that will benefit from compression and multi-record encoding...";
    
    auto result = client.send_text(large_message);
    
    if (result) {
        std::cout << "Advanced send successful:" << std::endl;
        std::cout << "  Bytes sent: " << result->bytes_sent << std::endl;
        std::cout << "  Fragments: " << result->fragments_sent << std::endl;
        std::cout << "  Record types: " << result->used_record_types.size() << std::endl;
        std::cout << "  Compression: " << (result->compression_used ? "enabled" : "disabled") << std::endl;
        std::cout << "  Latency: " << result->latency.count() << "ms" << std::endl;
    } else {
        std::cerr << "Advanced send failed" << std::endl;
    }
    
    return 0;
}
```

### Binary Data Transmission

```cpp
#include "chimera/client.hpp"
#include <fstream>

int main() {
    // Read binary file
    std::ifstream file("data.bin", std::ios::binary);
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
    
    // Configure for binary transmission
    chimera::ClientConfig config;
    config.encoding_strategy = chimera::EncodingStrategy::MULTI_RECORD;
    config.use_compression = true;  // Helpful for binary data
    config.max_fragments = 10;
    
    chimera::ChimeraClient client(config);
    
    // Send binary data
    auto result = client.send_data(data);
    
    if (result) {
        std::cout << "Binary data sent: " << data.size() << " bytes" << std::endl;
        std::cout << "Transmitted as: " << result->bytes_sent << " bytes" << std::endl;
        if (result->compression_used) {
            double ratio = 1.0 - static_cast<double>(result->bytes_sent) / data.size();
            std::cout << "Compression ratio: " << (ratio * 100) << "%" << std::endl;
        }
    }
    
    return 0;
}
```

### Async Operations (Phase 2)

```cpp
#include "chimera/client.hpp"
#include <future>

int main() {
    chimera::ClientConfig config;
    config.adaptive_transport = true;
    
    chimera::ChimeraClient client(config);
    
    // Send multiple messages asynchronously
    std::vector<std::future<void>> futures;
    
    for (int i = 0; i < 5; ++i) {
        futures.push_back(std::async(std::launch::async, [&client, i]() {
            std::string message = "Async message " + std::to_string(i);
            auto result = client.send_text(message);
            
            if (result) {
                std::cout << "Async message " << i << " sent successfully" << std::endl;
            }
        }));
    }
    
    // Wait for all operations to complete
    for (auto& future : futures) {
        future.wait();
    }
    
    return 0;
}
```

### Network Testing

```cpp
#include "chimera/client.hpp"

int main() {
    chimera::ClientConfig config;
    config.dns_server = "8.8.8.8";
    
    chimera::ChimeraClient client(config);
    
    // Test network connectivity
    auto ping_result = client.ping_dns_server();
    
    if (ping_result) {
        std::cout << "DNS server reachable in " << ping_result->count() << "ms" << std::endl;
        
        // Estimate capacity
        size_t capacity = client.estimate_capacity();
        std::cout << "Estimated capacity: " << capacity << " bytes" << std::endl;
        
    } else {
        std::cerr << "Cannot reach DNS server" << std::endl;
    }
    
    return 0;
}
```

## Thread Safety

CHIMERA clients are **not thread-safe**. If you need to use CHIMERA from multiple threads, create separate client instances for each thread or use appropriate synchronization mechanisms.

```cpp
// Good: Separate clients per thread
std::thread t1([config]() {
    chimera::ChimeraClient client1(config);
    client1.send_text("Message from thread 1");
});

std::thread t2([config]() {
    chimera::ChimeraClient client2(config);
    client2.send_text("Message from thread 2");
});
```

## Performance Considerations

- **Compression**: Enable for large payloads (>100 bytes typically)
- **Multi-record encoding**: Best for payloads >50 bytes
- **Fragment limits**: Keep max_fragments reasonable (3-10) for performance
- **Noise ratio**: Higher ratios increase overhead but improve evasion
- **Transport selection**: DoH has higher latency but better evasion than UDP

## Memory Usage

CHIMERA is designed to be memory-efficient:
- Minimal memory allocation during normal operations
- Automatic cleanup using RAII
- No persistent state between operations
- Configurable limits to prevent memory exhaustion

## Current Performance Metrics

Based on unified test suite results:
- **Encoding Performance**: ~2.2ms for 100 operations
- **Transport Creation**: ~462μs for 100 instances  
- **Capacity Estimation**: ~2μs for 1000 operations
- **Estimated Capacity**: ~220 bytes per message
- **Test Coverage**: 15/15 tests passing