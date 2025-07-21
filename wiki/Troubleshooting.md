# Troubleshooting

Common issues and solutions for CHIMERA framework.

## Table of Contents

- [Installation Issues](#installation-issues)
- [Build Problems](#build-problems)
- [Runtime Errors](#runtime-errors)
- [Network Issues](#network-issues)
- [Performance Problems](#performance-problems)
- [Security Concerns](#security-concerns)
- [Debugging Tools](#debugging-tools)

## Installation Issues

### Problem: CMake Cannot Find Dependencies

**Symptoms**:
```
CMake Error: Could not find a package configuration file provided by "PkgConfig"
-- Could NOT find liboqs (missing: liboqs_LIBRARIES liboqs_INCLUDE_DIRS)
```

**Solutions**:

1. **Install missing dependencies**:
   ```bash
   # Ubuntu/Debian
   sudo apt install pkg-config libsodium-dev liboqs-dev libcurl4-openssl-dev
   
   # macOS
   brew install pkg-config libsodium liboqs curl openssl
   
   # CentOS/RHEL
   sudo yum install pkgconfig libsodium-devel libcurl-devel openssl-devel
   ```

2. **Specify custom paths**:
   ```bash
   cmake -DCMAKE_PREFIX_PATH="/usr/local;/opt/homebrew" ..
   ```

3. **Build liboqs from source** (if not available in package manager):
   ```bash
   git clone https://github.com/open-quantum-safe/liboqs.git
   cd liboqs
   mkdir build && cd build
   cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
   make -j$(nproc) && sudo make install
   ```

### Problem: OpenSSL Version Conflicts (macOS)

**Symptoms**:
```
OpenSSL version mismatch
Could not find OpenSSL
```

**Solutions**:

1. **Use Homebrew OpenSSL**:
   ```bash
   brew install openssl
   cmake -DOPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl ..
   ```

2. **Set environment variables**:
   ```bash
   export OPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl
   export PKG_CONFIG_PATH="/opt/homebrew/opt/openssl/lib/pkgconfig"
   ```

### Problem: Compiler Too Old

**Symptoms**:
```
error: 'std::optional' was not declared in this scope
C++17 features not supported
```

**Solutions**:

1. **Update compiler**:
   ```bash
   # Ubuntu
   sudo apt install gcc-9 g++-9
   cmake -DCMAKE_CXX_COMPILER=g++-9 ..
   
   # macOS
   xcode-select --install
   ```

2. **Specify C++17 explicitly**:
   ```bash
   cmake -DCMAKE_CXX_STANDARD=17 ..
   ```

## Build Problems

### Problem: Linking Errors

**Symptoms**:
```
undefined reference to 'OQS_KEM_new'
undefined reference to 'sodium_init'
```

**Solutions**:

1. **Check library installation**:
   ```bash
   pkg-config --libs libsodium
   pkg-config --libs liboqs
   ```

2. **Update CMake cache**:
   ```bash
   rm -rf build/
   mkdir build && cd build
   cmake ..
   ```

3. **Manually specify libraries**:
   ```bash
   cmake -DCMAKE_EXE_LINKER_FLAGS="-lsodium -loqs -lcurl -lssl -lcrypto" ..
   ```

### Problem: Header Not Found

**Symptoms**:
```
fatal error: 'oqs/oqs.h' file not found
fatal error: 'sodium.h' file not found
```

**Solutions**:

1. **Check include paths**:
   ```bash
   find /usr -name "oqs.h" 2>/dev/null
   find /usr -name "sodium.h" 2>/dev/null
   ```

2. **Add include directories**:
   ```bash
   cmake -DCMAKE_CXX_FLAGS="-I/usr/local/include" ..
   ```

## Runtime Errors

### Problem: "libsodium initialization failed"

**Symptoms**:
```
FATAL: libsodium initialization failed!
Sodium initialization failed
```

**Solutions**:

1. **Check libsodium installation**:
   ```bash
   ldd ./chimera_demo | grep sodium
   ```

2. **Set library path**:
   ```bash
   export LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"
   ```

3. **Verify libsodium version**:
   ```bash
   pkg-config --modversion libsodium  # Should be 1.0.18+
   ```

### Problem: "ML-KEM768 not available in liboqs"

**Symptoms**:
```
ML-KEM768 not available in liboqs
Kyber768 not found
```

**Solutions**:

1. **Check liboqs algorithms**:
   ```cpp
   #include <oqs/oqs.h>
   
   void check_algorithms() {
       printf("Available KEMs:\n");
       for (size_t i = 0; i < OQS_KEM_algs_length; i++) {
           printf("  %s\n", OQS_KEM_alg_identifier(i));
       }
   }
   ```

2. **Build liboqs with Kyber support**:
   ```bash
   cmake -DOQS_ENABLE_KEM_kyber=ON ..
   ```

3. **Use alternative algorithm**:
   ```cpp
   // Check available algorithms and use fallback
   if (!OQS_KEM_alg_is_enabled(OQS_KEM_alg_kyber_768)) {
       // Use ML-KEM-512 or another available algorithm
   }
   ```

### Problem: DNS Query Failures

**Symptoms**:
```
NetworkError: DNS query failed
Socket creation failed
Invalid server IP address
```

**Solutions**:

1. **Test DNS connectivity**:
   ```bash
   nslookup example.com 8.8.8.8
   dig @8.8.8.8 example.com
   ```

2. **Check firewall rules**:
   ```bash
   # Allow DNS traffic
   sudo ufw allow out 53
   sudo ufw allow out 443  # For DoH
   sudo ufw allow out 853  # For DoT
   ```

3. **Try alternative DNS servers**:
   ```cpp
   config.server_ip = "1.1.1.1";      // Cloudflare
   config.server_ip = "208.67.222.222"; // OpenDNS
   ```

## Network Issues

### Problem: DoH/DoT Failures

**Symptoms**:
```
DoH transport failed
TLS handshake failed
HTTPS connection error
```

**Solutions**:

1. **Test HTTPS connectivity**:
   ```bash
   curl -v "https://cloudflare-dns.com/dns-query"
   ```

2. **Check TLS certificates**:
   ```bash
   openssl s_client -connect 1.1.1.1:853
   ```

3. **Use different DoH/DoT providers**:
   ```cpp
   // Google DoH
   config.server_ip = "8.8.8.8";
   
   // Cloudflare DoH/DoT
   config.server_ip = "1.1.1.1";
   
   // Quad9 DoH/DoT
   config.server_ip = "9.9.9.9";
   ```

### Problem: Timeout Errors

**Symptoms**:
```
TimeoutError: Operation timed out
DNS query timeout
```

**Solutions**:

1. **Increase timeout**:
   ```cpp
   config.timeout = std::chrono::milliseconds(15000);  // 15 seconds
   ```

2. **Test network latency**:
   ```bash
   ping 8.8.8.8
   traceroute 8.8.8.8
   ```

3. **Use faster DNS servers**:
   ```cpp
   // Use geographically closer DNS servers
   config.server_ip = "local-dns-server-ip";
   ```

### Problem: Blocked DNS Traffic

**Symptoms**:
```
Connection refused
Network unreachable
DNS blocked by firewall
```

**Solutions**:

1. **Switch to DoH transport**:
   ```cpp
   config.transport = chimera::TransportType::DoH;  // Uses HTTPS port 443
   ```

2. **Use alternative ports** (for UDP):
   ```cpp
   // Some networks allow DNS on alternative ports
   // Modify transport implementation to use port 5353 or 8053
   ```

3. **Test with different domains**:
   ```cpp
   config.target_domain = "google.com";  // Try well-known domains
   ```

## Performance Problems

### Problem: Slow Message Sending

**Symptoms**:
- High latency (>5 seconds per message)
- Poor throughput
- CPU usage spikes

**Solutions**:

1. **Optimize transport selection**:
   ```cpp
   config.transport = chimera::TransportType::UDP;  // Fastest
   config.behavioral_profile = chimera::BehavioralProfile::Gaming;  // Low latency
   config.timing_variance = std::chrono::milliseconds(0);  // No jitter
   ```

2. **Use async operations**:
   ```cpp
   chimera::AsyncIOManager io_manager;
   chimera::AsyncChimeraClient async_client(config, io_manager);
   
   // Send multiple messages concurrently
   std::vector<std::future<chimera::SendResult>> futures;
   for (const auto& message : messages) {
       futures.push_back(async_client.send_text_async(message));
   }
   ```

3. **Profile performance**:
   ```cpp
   auto start = std::chrono::high_resolution_clock::now();
   auto result = client.send_text(message);
   auto end = std::chrono::high_resolution_clock::now();
   auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
   std::cout << "Send took: " << duration.count() << "ms" << std::endl;
   ```

### Problem: Memory Leaks

**Symptoms**:
- Increasing memory usage over time
- System becomes unresponsive

**Solutions**:

1. **Check for resource leaks**:
   ```bash
   # Use Valgrind on Linux
   valgrind --leak-check=full ./chimera_demo
   
   # Use AddressSanitizer
   cmake -DCMAKE_CXX_FLAGS="-fsanitize=address -g" ..
   ```

2. **Proper cleanup**:
   ```cpp
   // Ensure clients are properly destroyed
   {
       chimera::ChimeraClient client(config);
       // Use client
   }  // Client automatically cleaned up here
   ```

## Security Concerns

### Problem: Traffic Analysis Detection

**Symptoms**:
- Consistent traffic patterns
- Easily identifiable DNS queries
- Timing correlation

**Solutions**:

1. **Enable behavioral mimicry**:
   ```cpp
   config.adaptive_transport = true;
   config.behavioral_profile = chimera::BehavioralProfile::Random;
   config.timing_variance = std::chrono::milliseconds(500);
   ```

2. **Use random subdomains**:
   ```cpp
   config.use_random_subdomains = true;
   config.target_domain = "legitimate-looking-domain.com";
   ```

3. **Vary message timing**:
   ```cpp
   chimera::BehavioralMimicry mimicry(chimera::BehavioralProfile::WebBrowsing);
   
   for (const auto& message : messages) {
       auto delay = mimicry.get_next_delay();
       std::this_thread::sleep_for(delay);
       client.send_text(message);
   }
   ```

### Problem: Weak Cryptography

**Symptoms**:
- Using legacy mode
- Disabled hybrid crypto

**Solutions**:

1. **Enable strong cryptography**:
   ```cpp
   config.use_hybrid_crypto = true;  // Always enable
   ```

2. **Verify crypto status**:
   ```cpp
   void verify_crypto_strength() {
       if (!config.use_hybrid_crypto) {
           std::cerr << "WARNING: Using legacy cryptography!" << std::endl;
           std::cerr << "Enable hybrid crypto for quantum resistance" << std::endl;
       }
   }
   ```

## Debugging Tools

### Verbose Logging

```cpp
#define CHIMERA_DEBUG 1

class DebugChimeraClient {
private:
    chimera::ChimeraClient client_;
    
public:
    DebugChimeraClient(const chimera::ClientConfig& config) : client_(config) {}
    
    bool send_text_debug(const std::string& message) {
        std::cout << "[DEBUG] Sending message: " << message.substr(0, 50) 
                  << (message.length() > 50 ? "..." : "") << std::endl;
        
        auto start = std::chrono::high_resolution_clock::now();
        auto result = client_.send_text(message);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        if (result) {
            std::cout << "[DEBUG] Success after " << duration.count() << "ms" << std::endl;
            std::cout << "[DEBUG] Response: " << result->response_summary << std::endl;
            return true;
        } else {
            std::cout << "[DEBUG] Failed after " << duration.count() << "ms" << std::endl;
            return false;
        }
    }
};
```

### Network Debugging

```cpp
void debug_network_connectivity(const chimera::ClientConfig& config) {
    std::cout << "=== Network Debug Information ===" << std::endl;
    std::cout << "Server IP: " << config.server_ip << std::endl;
    std::cout << "Target Domain: " << config.target_domain << std::endl;
    std::cout << "Transport: " << static_cast<int>(config.transport) << std::endl;
    std::cout << "Timeout: " << config.timeout.count() << "ms" << std::endl;
    
    // Test basic connectivity
    chimera::ChimeraClient client(config);
    auto ping_result = client.ping();
    
    if (ping_result) {
        std::cout << "✓ Basic connectivity OK" << std::endl;
    } else {
        std::cout << "✗ Basic connectivity FAILED" << std::endl;
    }
}
```

### Configuration Validation

```cpp
bool validate_configuration(const chimera::ClientConfig& config) {
    bool valid = true;
    
    if (config.server_ip.empty()) {
        std::cerr << "ERROR: server_ip is empty" << std::endl;
        valid = false;
    }
    
    if (config.target_domain.empty()) {
        std::cerr << "ERROR: target_domain is empty" << std::endl;
        valid = false;
    }
    
    if (config.timeout.count() < 1000) {
        std::cerr << "WARNING: timeout < 1s may cause issues" << std::endl;
    }
    
    if (!config.use_hybrid_crypto) {
        std::cerr << "WARNING: hybrid crypto disabled (security risk)" << std::endl;
    }
    
    return valid;
}
```

## Getting Help

### Collecting Debug Information

When reporting issues, include:

1. **System information**:
   ```bash
   uname -a
   gcc --version
   cmake --version
   ```

2. **Library versions**:
   ```bash
   pkg-config --modversion libsodium
   pkg-config --modversion liboqs
   ```

3. **Build output**:
   ```bash
   cd build
   make VERBOSE=1 > build.log 2>&1
   ```

4. **Runtime logs**:
   ```bash
   strace -o trace.log ./chimera_demo "test message"
   ```

### Common Debug Commands

```bash
# Test DNS resolution
nslookup example.com 8.8.8.8

# Test network connectivity
telnet 8.8.8.8 53
curl -v "https://cloudflare-dns.com/dns-query"

# Check library dependencies
ldd ./chimera_demo

# Monitor network traffic
sudo tcpdump -i any port 53 or port 443 or port 853

# Check system logs
journalctl -f | grep chimera
tail -f /var/log/syslog | grep DNS
```

---

**Next**: Check [FAQ](FAQ.md) for frequently asked questions, or return to [Home](Home.md) for main documentation.