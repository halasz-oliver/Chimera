# API Reference

Complete API documentation for the CHIMERA framework.

## Core Classes

### ChimeraClient

Main client class for sending steganographic messages.

```cpp
namespace chimera {
    class ChimeraClient {
    public:
        explicit ChimeraClient(ClientConfig config);
        
        // Message sending
        tl::expected<SendResult, ChimeraError> send_text(const std::string& message) const;
        tl::expected<std::chrono::milliseconds, ChimeraError> ping_dns_server() const;
        
        // Configuration
        const ClientConfig& get_config() const;
        void update_config(ClientConfig new_config);
    };
}
```

#### Constructor
```cpp
explicit ChimeraClient(ClientConfig config);
```
**Parameters**:
- `config`: Configuration structure defining behavior

#### send_text()
```cpp
tl::expected<SendResult, ChimeraError> send_text(const std::string& message) const;
```
**Parameters**:
- `message`: Text message to send (will be encrypted automatically)

**Returns**:
- `SendResult` on success
- `ChimeraError` on failure

#### ping_dns_server()
```cpp
tl::expected<std::chrono::milliseconds, ChimeraError> ping_dns_server() const;
```
**Returns**:
- Ping time in milliseconds on success
- `ChimeraError` on failure

### ClientConfig

Configuration structure for CHIMERA clients.

```cpp
struct ClientConfig {
    std::string dns_server = "8.8.8.8";
    uint16_t dns_port = 53;
    std::string target_domain = "example.com";
    std::chrono::milliseconds timeout{5000};
    bool use_random_subdomains = true;
    bool use_hybrid_crypto = true;
    TransportType transport = TransportType::UDP;
    bool adaptive_transport = false;
    std::chrono::milliseconds timing_variance{100};
    BehavioralProfile behavioral_profile = BehavioralProfile::Normal;
};
```

## Enumerations

### TransportType
```cpp
enum class TransportType {
    UDP,    // Traditional DNS over UDP:53
    DoH,    // DNS-over-HTTPS (RFC 8484)
    DoT     // DNS-over-TLS (RFC 7858)
};
```

### BehavioralProfile
```cpp
enum class BehavioralProfile {
    Normal,      // Regular DNS queries
    WebBrowsing, // Web browsing patterns
    Enterprise,  // Corporate network patterns
    Gaming,      // Gaming traffic patterns
    Random       // Random pattern selection
};
```

### ChimeraError
```cpp
enum class ChimeraError {
    NetworkError,      // Network connectivity issues
    ConfigError,       // Invalid configuration
    EncodingError,     // Message encoding failures
    TimeoutError,      // Operation timeout
    DnsError,         // DNS-specific errors
    CryptoError       // Cryptographic operation failures
};
```

## Result Types

### SendResult
```cpp
struct SendResult {
    size_t bytes_sent;                     // Number of bytes transmitted
    std::chrono::milliseconds latency;     // Time taken for response
    std::string used_domain;               // Domain name used in query
};
```

## Behavioral Mimicry (Advanced)

### BehavioralMimicry

```cpp
namespace chimera {
    class BehavioralMimicry {
    public:
        explicit BehavioralMimicry(BehavioralProfile profile);
        
        void apply_behavioral_delay() const;
        bool should_switch_transport() const;
        TransportType get_recommended_transport() const;
        void set_profile(BehavioralProfile profile);
        const TrafficPattern& get_pattern() const;
    };
}
```

### AdaptiveTransportManager

```cpp
namespace chimera {
    class AdaptiveTransportManager {
    public:
        AdaptiveTransportManager();
        
        void add_transport(TransportType transport);
        TransportType get_next_transport(bool random = true) const;
        void force_switch();
        bool should_switch() const;
    };
}
```

## Async I/O (Advanced)

### AsyncChimeraClient

```cpp
namespace chimera {
    class AsyncChimeraClient {
    public:
        explicit AsyncChimeraClient(ClientConfig config);
        
        void send_text_async(const std::string& message, AsyncCallback callback);
        std::future<AsyncResult> send_text_future(const std::string& message);
        
        void start();
        void stop();
        
        const ClientConfig& get_config() const;
        void update_config(ClientConfig new_config);
    };
}
```

## Error Handling

All CHIMERA APIs use `tl::expected<T, Error>` for error handling:

```cpp
// Check for success
auto result = client.send_text("message");
if (result) {
    // Success - use result.value() or *result
    std::cout << "Success: " << result->bytes_sent << " bytes sent" << std::endl;
} else {
    // Error - use result.error()
    std::cerr << "Error occurred" << std::endl;
}
```

## Thread Safety

- **ChimeraClient**: Thread-safe for read operations, requires external synchronization for config updates
- **AsyncChimeraClient**: Fully thread-safe
- **BehavioralMimicry**: Thread-safe

---

**Note**: This API reference covers the current stable implementation. Advanced features may have additional complexity considerations.