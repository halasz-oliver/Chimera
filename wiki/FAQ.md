# Frequently Asked Questions (FAQ)

Common questions and answers about the CHIMERA framework.

## General Questions

### What is CHIMERA?

**CHIMERA** (Covert Hybrid Intelligence via Masked Encrypted Radio Applications) is a production-ready steganographic communication framework that hides encrypted messages inside DNS queries. It uses post-quantum cryptography (X25519 + ML-KEM768) to ensure security against both classical and quantum attacks.

### Is CHIMERA legal to use?

The legality of CHIMERA depends on your jurisdiction and use case:

- **Research and education**: Generally legal worldwide
- **Personal privacy**: Legal in most jurisdictions
- **Corporate security testing**: Legal with proper authorization
- **Malicious use**: Illegal and not supported

**Always comply with local laws and regulations. Obtain proper authorization before using CHIMERA in corporate or production environments.**

### How does DNS steganography work?

CHIMERA embeds encrypted messages in DNS TXT record queries:

1. **Message encryption**: Text is encrypted with ChaCha20-Poly1305
2. **Base64 encoding**: Encrypted data is encoded for DNS compatibility  
3. **DNS embedding**: Encoded data becomes part of the DNS query
4. **Steganographic transmission**: Query looks like legitimate DNS traffic

Example:
```
Original: "Secret message"
Encrypted: [binary data]
Encoded: "U2VjcmV0IG1lc3NhZ2U="
DNS Query: "U2VjcmV0IG1lc3NhZ2U=.example.com"
```

### What makes CHIMERA "quantum-resistant"?

CHIMERA uses hybrid post-quantum cryptography:

- **X25519**: Elliptic curve Diffie-Hellman (classical security)
- **ML-KEM768**: Post-quantum key encapsulation mechanism
- **Hybrid approach**: Combines both for maximum security

Even if quantum computers break X25519, ML-KEM768 provides continued protection.

## Technical Questions

### What DNS servers work with CHIMERA?

CHIMERA works with any standard DNS server:

**Recommended servers**:
- **Google**: 8.8.8.8, 8.8.4.4
- **Cloudflare**: 1.1.1.1, 1.0.0.1  
- **Quad9**: 9.9.9.9, 149.112.112.112
- **OpenDNS**: 208.67.222.222, 208.67.220.220

**Transport support**:
- **UDP**: All DNS servers
- **DoH**: Google, Cloudflare, Quad9, others with HTTPS support
- **DoT**: Cloudflare, Quad9, others with TLS support

### What's the difference between UDP, DoH, and DoT?

| Feature | UDP | DoH | DoT |
|---------|-----|-----|-----|
| **Encryption** | None | HTTPS (TLS) | TLS |
| **Port** | 53 | 443 | 853 |
| **Stealth** | Low | High | Medium |
| **Performance** | Best | Good | Good |
| **Blocking resistance** | Low | High | Medium |

**Recommendations**:
- **Development/Testing**: UDP
- **Production/Stealth**: DoH
- **Corporate environments**: DoT

### How large can messages be?

DNS has inherent size limitations:

- **UDP DNS**: ~512 bytes total packet size
- **TCP DNS**: ~65KB theoretical, ~4KB practical
- **Effective payload**: ~200-400 characters per message

**For larger data**:
- Split into multiple messages
- Use compression
- Implement message segmentation

```cpp
// Example: Split large message
std::vector<std::string> split_message(const std::string& large_message) {
    std::vector<std::string> chunks;
    const size_t chunk_size = 200;
    
    for (size_t i = 0; i < large_message.length(); i += chunk_size) {
        chunks.push_back(large_message.substr(i, chunk_size));
    }
    
    return chunks;
}
```

### Can CHIMERA be detected?

CHIMERA is designed to be stealthy, but no system is completely undetectable:

**Detection methods**:
- **Traffic analysis**: Unusual DNS query patterns
- **Content analysis**: Random-looking subdomain names
- **Timing analysis**: Regular intervals between queries
- **Volume analysis**: High DNS query frequency

**Evasion techniques**:
- **Behavioral mimicry**: Mimic legitimate user patterns
- **Random timing**: Variable delays between messages
- **Transport mixing**: Switch between UDP/DoH/DoT
- **Domain blending**: Use popular domain names

### What platforms does CHIMERA support?

**Operating Systems**:
- **Linux**: Ubuntu 18.04+, CentOS 7+, Debian 10+
- **macOS**: 10.15+ (with Homebrew dependencies)
- **Windows**: Windows 10+ (with MSYS2/MinGW or Visual Studio)

**Architectures**:
- **x86_64**: Fully supported
- **ARM64**: Supported (Apple Silicon, Raspberry Pi 4+)
- **ARM32**: Limited support (may require manual dependency building)

**Dependencies**:
- **C++17 compiler**: GCC 8+, Clang 10+, MSVC 2019+
- **CMake**: 3.15+
- **Libraries**: libsodium, liboqs, libcurl, OpenSSL

## Security Questions

### Is CHIMERA secure against surveillance?

CHIMERA provides multiple layers of security:

**Cryptographic security**:
- **Quantum-resistant**: ML-KEM768 post-quantum crypto
- **Authenticated encryption**: ChaCha20-Poly1305 AEAD
- **Forward secrecy**: Fresh keys for each session

**Traffic security**:
- **Encrypted transport**: DoH/DoT hide DNS content
- **Behavioral mimicry**: Traffic patterns match legitimate usage
- **Random subdomains**: Avoid detectable patterns

**Limitations**:
- **Metadata**: DNS queries still create network metadata
- **Endpoint security**: Client/server security is critical
- **Implementation attacks**: Side-channel vulnerabilities possible

### How do I verify message integrity?

CHIMERA provides automatic integrity verification:

```cpp
auto result = client.send_text("Important message");
if (result) {
    // Message was successfully sent AND verified
    std::cout << "Message integrity confirmed" << std::endl;
} else {
    // Either send failed OR integrity check failed
    std::cout << "Message integrity compromised" << std::endl;
}
```

**Integrity mechanisms**:
- **AEAD encryption**: ChaCha20-Poly1305 includes authentication
- **DNS validation**: Response parsing includes integrity checks
- **Transport security**: DoH/DoT provide additional integrity

### What happens if keys are compromised?

**CHIMERA's defense mechanisms**:

1. **Perfect Forward Secrecy**: Old messages remain secure
   ```cpp
   // Each session uses fresh keys
   chimera::HybridKeyExchange::generate_keypair();  // New keys each time
   ```

2. **Key rotation**: Generate new keys regularly
   ```cpp
   void rotate_keys_periodically() {
       auto new_keypair = chimera::HybridKeyExchange::generate_keypair();
       // Update client configuration with new keys
   }
   ```

3. **Compromise recovery**: Quantum-resistant algorithms
   ```cpp
   config.use_hybrid_crypto = true;  // Always enable post-quantum crypto
   ```

## Usage Questions

### How do I send binary data?

CHIMERA automatically handles binary data encoding:

```cpp
// Read binary file
std::ifstream file("binary_data.bin", std::ios::binary);
std::string binary_data((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

// Send binary data (automatically base64 encoded)
auto result = client.send_text(binary_data);
```

**For large binary files**:
```cpp
void send_binary_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    std::string chunk;
    chunk.reserve(200);  // Chunk size for DNS
    
    while (file.read(&chunk[0], 200) || file.gcount() > 0) {
        chunk.resize(file.gcount());
        auto result = client.send_text(chunk);
        if (!result) {
            std::cerr << "Failed to send chunk" << std::endl;
            break;
        }
    }
}
```

### How do I implement bidirectional communication?

CHIMERA currently supports unidirectional communication (client to server). For bidirectional:

**Option 1: Dual clients**
```cpp
// Client A sends to domain-a.com
chimera::ClientConfig config_a;
config_a.target_domain = "channel-a.example.com";
chimera::ChimeraClient client_a(config_a);

// Client B sends to domain-b.com  
chimera::ClientConfig config_b;
config_b.target_domain = "channel-b.example.com";
chimera::ChimeraClient client_b(config_b);

// A sends to B
client_a.send_text("Message from A to B");

// B sends to A
client_b.send_text("Message from B to A");
```

**Option 2: Polling pattern**
```cpp
void bidirectional_communication() {
    // Send outgoing message
    client.send_text("Outgoing message");
    
    // Poll for incoming messages (implementation dependent)
    // This requires server-side support for message storage/retrieval
    auto incoming = poll_for_messages();
    if (incoming) {
        process_incoming_message(*incoming);
    }
}
```

### How do I integrate CHIMERA with my application?

**Simple integration**:
```cpp
#include "chimera/client.hpp"

class MyApplication {
private:
    chimera::ChimeraClient chimera_client_;
    
public:
    MyApplication() : chimera_client_(create_config()) {}
    
    void send_alert(const std::string& alert_message) {
        auto result = chimera_client_.send_text("ALERT: " + alert_message);
        if (!result) {
            // Fallback to other communication method
            send_via_email(alert_message);
        }
    }
    
private:
    chimera::ClientConfig create_config() {
        chimera::ClientConfig config;
        config.server_ip = get_dns_server_from_settings();
        config.target_domain = get_target_domain_from_settings();
        config.transport = chimera::TransportType::DoH;
        return config;
    }
};
```

**Advanced integration with error handling**:
```cpp
class RobustChimeraWrapper {
private:
    chimera::ChimeraClient client_;
    std::queue<std::string> retry_queue_;
    
public:
    bool send_with_retry(const std::string& message, int max_retries = 3) {
        for (int attempt = 0; attempt < max_retries; ++attempt) {
            auto result = client_.send_text(message);
            if (result) {
                return true;
            }
            
            // Exponential backoff
            std::this_thread::sleep_for(std::chrono::seconds(1 << attempt));
        }
        
        // Queue for later retry
        retry_queue_.push(message);
        return false;
    }
    
    void process_retry_queue() {
        while (!retry_queue_.empty()) {
            std::string message = retry_queue_.front();
            retry_queue_.pop();
            
            if (client_.send_text(message)) {
                std::cout << "Retry successful for queued message" << std::endl;
            } else {
                // Put back in queue or handle failure
                retry_queue_.push(message);
                break;  // Stop processing if still failing
            }
        }
    }
};
```

## Performance Questions

### What's the typical throughput?

Performance depends on configuration and network conditions:

**UDP Transport**:
- **Local network**: 50-100 messages/second
- **Internet**: 10-50 messages/second
- **Latency**: 10-100ms per message

**DoH/DoT Transport**:
- **Local network**: 20-50 messages/second  
- **Internet**: 5-20 messages/second
- **Latency**: 50-500ms per message

**Optimization tips**:
```cpp
// High-performance configuration
config.transport = chimera::TransportType::UDP;
config.timeout = std::chrono::milliseconds(2000);
config.behavioral_profile = chimera::BehavioralProfile::Gaming;
config.timing_variance = std::chrono::milliseconds(0);
config.adaptive_transport = false;
```

### How do I optimize for speed vs stealth?

**Speed-optimized configuration**:
```cpp
chimera::ClientConfig speed_config;
speed_config.transport = chimera::TransportType::UDP;        // Fastest
speed_config.use_random_subdomains = false;                 // Less overhead
speed_config.behavioral_profile = chimera::BehavioralProfile::Gaming;
speed_config.timing_variance = std::chrono::milliseconds(0); // No delays
speed_config.adaptive_transport = false;                    // No switching
```

**Stealth-optimized configuration**:
```cpp
chimera::ClientConfig stealth_config;
stealth_config.transport = chimera::TransportType::DoH;      // Encrypted
stealth_config.use_random_subdomains = true;                // Avoid patterns
stealth_config.behavioral_profile = chimera::BehavioralProfile::Random;
stealth_config.timing_variance = std::chrono::milliseconds(500); // High variance
stealth_config.adaptive_transport = true;                   // Transport mixing
```

**Balanced configuration**:
```cpp
chimera::ClientConfig balanced_config;
balanced_config.transport = chimera::TransportType::DoT;     // Good speed + security
balanced_config.behavioral_profile = chimera::BehavioralProfile::Normal;
balanced_config.timing_variance = std::chrono::milliseconds(100);
balanced_config.adaptive_transport = true;
```

## Troubleshooting Questions

### Why are my messages failing to send?

**Common causes and solutions**:

1. **Network connectivity**:
   ```bash
   # Test basic DNS
   nslookup example.com 8.8.8.8
   
   # Test DoH
   curl "https://cloudflare-dns.com/dns-query?name=example.com"
   ```

2. **Firewall blocking**:
   ```bash
   # Allow DNS ports
   sudo ufw allow out 53    # UDP DNS
   sudo ufw allow out 443   # DoH
   sudo ufw allow out 853   # DoT
   ```

3. **Configuration issues**:
   ```cpp
   // Validate configuration
   if (config.server_ip.empty() || config.target_domain.empty()) {
       std::cerr << "Invalid configuration" << std::endl;
   }
   ```

4. **Timeout too short**:
   ```cpp
   config.timeout = std::chrono::milliseconds(10000);  // Increase timeout
   ```

### Why is CHIMERA slow?

**Performance troubleshooting**:

1. **Check transport type**:
   ```cpp
   // UDP is fastest
   config.transport = chimera::TransportType::UDP;
   ```

2. **Reduce behavioral delays**:
   ```cpp
   config.timing_variance = std::chrono::milliseconds(0);
   config.behavioral_profile = chimera::BehavioralProfile::Gaming;
   ```

3. **Use async operations**:
   ```cpp
   chimera::AsyncChimeraClient async_client(config, io_manager);
   auto future = async_client.send_text_async(message);
   ```

4. **Measure performance**:
   ```cpp
   auto start = std::chrono::high_resolution_clock::now();
   auto result = client.send_text(message);
   auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
       std::chrono::high_resolution_clock::now() - start);
   std::cout << "Send took: " << duration.count() << "ms" << std::endl;
   ```

## Development Questions

### How do I contribute to CHIMERA?

**Getting started**:
1. Fork the repository
2. Set up development environment
3. Read the contribution guidelines
4. Submit pull requests with tests

**Development setup**:
```bash
git clone https://github.com/your-fork/chimera.git
cd chimera
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
```

### Can I modify CHIMERA for my needs?

Yes! CHIMERA is designed to be extensible:

**Custom transports**:
```cpp
class MyCustomTransport : public chimera::ITransport {
public:
    tl::expected<std::vector<uint8_t>, chimera::TransportError> 
    send_query(const std::vector<uint8_t>& query) override {
        // Your custom transport implementation
    }
};
```

**Custom behavioral profiles**:
```cpp
class MyBehavioralMimicry : public chimera::BehavioralMimicry {
public:
    std::chrono::milliseconds get_next_delay() override {
        // Your custom timing logic
    }
};
```

### How do I add new features?

**Development process**:
1. **Design**: Document the feature and API
2. **Implement**: Add code with comprehensive tests
3. **Test**: Ensure all existing tests pass
4. **Document**: Update wiki and examples
5. **Submit**: Create pull request with description

**Testing requirements**:
```bash
# Run all tests
make chimera_test chimera_test_comprehensive
./chimera_test
./chimera_test_comprehensive

# Run with sanitizers
cmake -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined" ..
make && ./chimera_test_comprehensive
```

---

**Still have questions?** Check the [Troubleshooting](Troubleshooting.md) guide or review the [API Reference](API-Reference.md) for detailed technical information.