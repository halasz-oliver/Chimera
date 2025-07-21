# Configuration Reference

This page provides complete documentation for all CHIMERA configuration options.

## ClientConfig Structure

The `ClientConfig` structure controls all aspects of CHIMERA's behavior:

```cpp
struct ClientConfig {
    // Network Configuration
    std::string dns_server = "8.8.8.8";
    uint16_t dns_port = 53;
    std::string target_domain = "example.com";
    std::chrono::milliseconds timeout{5000};
    
    // Security Configuration
    bool use_random_subdomains = true;
    bool use_hybrid_crypto = true;
    
    // Transport Configuration
    TransportType transport = TransportType::UDP;
    
    // Phase 2 - Behavioral Mimicry
    bool adaptive_transport = false;
    std::chrono::milliseconds timing_variance{100};
    BehavioralProfile behavioral_profile = BehavioralProfile::Normal;
};
```

## Network Configuration

### dns_server
**Type**: `std::string`  
**Default**: `"8.8.8.8"`  
**Description**: IP address of the DNS server to use.

```cpp
config.dns_server = "8.8.8.8";     // Google DNS
config.dns_server = "1.1.1.1";     // Cloudflare DNS
config.dns_server = "208.67.222.222"; // OpenDNS
```

**Popular DNS Servers**:
| Provider | Primary | Secondary | Features |
|----------|---------|-----------|----------|
| Google | 8.8.8.8 | 8.8.4.4 | Fast, reliable |
| Cloudflare | 1.1.1.1 | 1.0.0.1 | Privacy-focused |
| OpenDNS | 208.67.222.222 | 208.67.220.220 | Filtering options |
| Quad9 | 9.9.9.9 | 149.112.112.112 | Security filtering |

### dns_port
**Type**: `uint16_t`  
**Default**: `53`  
**Description**: Port number for DNS communication.

```cpp
config.dns_port = 53;      // Standard DNS port
config.dns_port = 5353;    // Alternative DNS port
```

### target_domain
**Type**: `std::string`  
**Default**: `"example.com"`  
**Description**: The domain name to use in DNS queries.

```cpp
config.target_domain = "example.com";       // Public test domain
config.target_domain = "your-domain.com";   // Your own domain
config.target_domain = "subdomain.example.com"; // Subdomain
```

**Best Practices**:
- Use domains you control for production
- `example.com` is safe for testing (RFC reserved)
- Avoid using others' domains without permission

### timeout
**Type**: `std::chrono::milliseconds`  
**Default**: `5000` (5 seconds)  
**Description**: Maximum time to wait for DNS responses.

```cpp
config.timeout = std::chrono::milliseconds(3000);   // 3 seconds
config.timeout = std::chrono::milliseconds(10000);  // 10 seconds
config.timeout = std::chrono::seconds(15);          // 15 seconds
```

**Guidelines**:
- **3-5 seconds**: Good for local/fast networks
- **5-10 seconds**: Recommended for internet usage
- **10+ seconds**: For slow or unreliable connections

## Security Configuration

### use_random_subdomains
**Type**: `bool`  
**Default**: `true`  
**Description**: Generate random subdomains for each query to avoid patterns.

```cpp
config.use_random_subdomains = true;   // Recommended
config.use_random_subdomains = false;  // Use target_domain directly
```

**When enabled**: 
- Queries go to: `random123.example.com`
- Each message uses different subdomain
- Harder to detect patterns

**When disabled**:
- All queries go to: `example.com`
- Simpler, but more detectable

### use_hybrid_crypto
**Type**: `bool`  
**Default**: `true`  
**Description**: Enable post-quantum hybrid key exchange (X25519 + ML-KEM768).

```cpp
config.use_hybrid_crypto = true;   // Quantum-resistant (recommended)
config.use_hybrid_crypto = false;  // Legacy mode (not recommended)
```

**Security Impact**:
- **Enabled**: Full post-quantum security
- **Disabled**: Classical cryptography only (vulnerable to quantum attacks)

## Transport Configuration

### transport
**Type**: `TransportType`  
**Default**: `TransportType::UDP`  
**Description**: Network protocol for DNS communication.

```cpp
enum class TransportType {
    UDP,    // Traditional DNS over UDP:53
    DoH,    // DNS-over-HTTPS (RFC 8484)
    DoT     // DNS-over-TLS (RFC 7858)
};
```

#### UDP Transport
```cpp
config.transport = TransportType::UDP;
```
- **Pros**: Fast, low latency, widely supported
- **Cons**: Unencrypted, easily detected and blocked
- **Use case**: Testing, development, unrestricted networks

#### DoH Transport (DNS-over-HTTPS)
```cpp
config.transport = TransportType::DoH;
```
- **Pros**: Encrypted, looks like web traffic, hard to block
- **Cons**: Higher latency, requires HTTPS support
- **Use case**: Production, restrictive networks, high stealth

#### DoT Transport (DNS-over-TLS)
```cpp
config.transport = TransportType::DoT;
```
- **Pros**: Encrypted, good performance, dedicated protocol
- **Cons**: Easily identifiable as DoT traffic
- **Use case**: Networks with DoT support, balanced security/performance

**Transport Comparison**:
| Feature | UDP | DoH | DoT |
|---------|-----|-----|-----|
| Encryption | ❌ | ✅ | ✅ |
| Stealth | Low | High | Medium |
| Performance | Best | Good | Good |
| Blocking Resistance | Low | High | Medium |
| Setup Complexity | Simple | Medium | Medium |

## Phase 2 - Behavioral Mimicry

### adaptive_transport
**Type**: `bool`  
**Default**: `false`  
**Description**: Enable dynamic transport switching based on behavioral profiles.

```cpp
config.adaptive_transport = true;   // Enable smart transport selection
config.adaptive_transport = false;  // Use fixed transport
```

**When enabled**:
- CHIMERA automatically switches between UDP/DoH/DoT
- Selection based on behavioral profile and network conditions
- Provides better evasion capabilities

### timing_variance
**Type**: `std::chrono::milliseconds`  
**Default**: `100` (100ms)  
**Description**: Random delay variance to mimic human behavior.

```cpp
config.timing_variance = std::chrono::milliseconds(50);   // Low variance
config.timing_variance = std::chrono::milliseconds(200);  // High variance
config.timing_variance = std::chrono::milliseconds(0);    // No jitter
```

**Impact**:
- Adds random delays between requests
- Makes traffic patterns less predictable
- Higher values = better evasion, lower performance

### behavioral_profile
**Type**: `BehavioralProfile`  
**Default**: `BehavioralProfile::Normal`  
**Description**: Predefined traffic patterns that mimic different user types.

```cpp
enum class BehavioralProfile {
    Normal,      // Balanced usage pattern
    WebBrowsing, // Frequent, bursty requests
    Enterprise,  // Regular, scheduled patterns  
    Gaming,      // Low-latency, consistent requests
    Random       // Unpredictable patterns
};
```

#### Profile Characteristics:

**Normal Profile**
```cpp
config.behavioral_profile = BehavioralProfile::Normal;
```
- Moderate request frequency
- Balanced timing patterns
- Good for general use

**WebBrowsing Profile**
```cpp
config.behavioral_profile = BehavioralProfile::WebBrowsing;
```
- Bursty traffic patterns
- Mimics web browsing behavior
- Prefers DoH transport

**Enterprise Profile**
```cpp
config.behavioral_profile = BehavioralProfile::Enterprise;
```
- Regular, predictable patterns
- Lower variance in timing
- Suitable for automated systems

**Gaming Profile**
```cpp
config.behavioral_profile = BehavioralProfile::Gaming;
```
- Low-latency requirements
- Consistent timing
- Prefers UDP transport

**Random Profile**
```cpp
config.behavioral_profile = BehavioralProfile::Random;
```
- Unpredictable patterns
- High timing variance
- Random transport selection

## Complete Configuration Examples

### Development Configuration
```cpp
chimera::ClientConfig dev_config() {
    chimera::ClientConfig config;
    config.dns_server = "8.8.8.8";
    config.target_domain = "example.com";
    config.transport = TransportType::UDP;
    config.timeout = std::chrono::milliseconds(3000);
    config.use_random_subdomains = false;  // Simpler for debugging
    config.adaptive_transport = false;
    return config;
}
```

### Production Configuration
```cpp
chimera::ClientConfig production_config() {
    chimera::ClientConfig config;
    config.dns_server = "1.1.1.1";  // Cloudflare for privacy
    config.target_domain = "your-domain.com";  // Your domain
    config.transport = TransportType::DoH;  // Encrypted transport
    config.timeout = std::chrono::milliseconds(10000);
    config.use_random_subdomains = true;
    config.use_hybrid_crypto = true;
    config.adaptive_transport = true;  // Enable evasion
    config.behavioral_profile = BehavioralProfile::WebBrowsing;
    config.timing_variance = std::chrono::milliseconds(200);
    return config;
}
```

### High-Stealth Configuration
```cpp
chimera::ClientConfig stealth_config() {
    chimera::ClientConfig config;
    config.server_ip = "1.1.1.1";
    config.target_domain = "popular-site.com";  // Blend with popular traffic
    config.transport = TransportType::DoH;
    config.timeout = std::chrono::milliseconds(15000);
    config.use_random_subdomains = true;
    config.use_hybrid_crypto = true;
    config.adaptive_transport = true;
    config.behavioral_profile = BehavioralProfile::Random;
    config.timing_variance = std::chrono::milliseconds(500);  // High variance
    return config;
}
```

### Performance Configuration
```cpp
chimera::ClientConfig performance_config() {
    chimera::ClientConfig config;
    config.server_ip = "8.8.8.8";
    config.target_domain = "example.com";
    config.transport = TransportType::UDP;  // Fastest transport
    config.timeout = std::chrono::milliseconds(2000);  // Short timeout
    config.use_random_subdomains = true;
    config.use_hybrid_crypto = true;
    config.adaptive_transport = false;  // Fixed transport
    config.behavioral_profile = BehavioralProfile::Gaming;  // Low latency
    config.timing_variance = std::chrono::milliseconds(50);  // Minimal jitter
    return config;
}
```

## Configuration Validation

CHIMERA provides built-in validation for configurations:

```cpp
bool validate_config(const chimera::ClientConfig& config) {
    // Check required fields
    if (config.server_ip.empty()) {
        std::cerr << "Error: server_ip is required" << std::endl;
        return false;
    }
    
    if (config.target_domain.empty()) {
        std::cerr << "Error: target_domain is required" << std::endl;
        return false;
    }
    
    // Validate timeout
    if (config.timeout.count() < 1000) {
        std::cerr << "Warning: timeout < 1s may cause issues" << std::endl;
    }
    
    return true;
}
```

## Environment Variables

CHIMERA can read configuration from environment variables:

```bash
export CHIMERA_SERVER_IP="1.1.1.1"
export CHIMERA_DOMAIN="your-domain.com"
export CHIMERA_TRANSPORT="doh"
export CHIMERA_TIMEOUT="10000"
```

```cpp
chimera::ClientConfig config_from_env() {
    chimera::ClientConfig config;
    
    if (const char* server = std::getenv("CHIMERA_SERVER_IP")) {
        config.server_ip = server;
    }
    
    if (const char* domain = std::getenv("CHIMERA_DOMAIN")) {
        config.target_domain = domain;
    }
    
    // ... additional environment variable handling
    
    return config;
}
```

## Next Steps

- **[Advanced Features](Advanced-Features.md)** - Phase 2 features and async I/O
- **[API Reference](API-Reference.md)** - Complete API documentation
- **[Examples](Examples.md)** - Real-world usage patterns

---

**Note**: Always validate your configuration before deployment, especially in production environments.