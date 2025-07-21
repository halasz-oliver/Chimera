# Basic Usage

This guide covers the fundamental operations of CHIMERA, from simple message sending to basic configuration.

## Quick Start Example

```cpp
#include "chimera/client.hpp"
#include <iostream>

int main() {
    // Create basic configuration
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
    
    return 0;
}
```

## Command Line Interface

CHIMERA provides a command-line demo for quick testing:

### Basic Commands

```bash
# Send a simple message
./chimera_demo "Your secret message"

# Send message from file
./chimera_demo message.txt

# Specify custom DNS server
./chimera_demo --server 1.1.1.1 "Message via Cloudflare DNS"

# Use custom domain
./chimera_demo --domain your-domain.com "Custom domain message"

# Use different transport
./chimera_demo --transport doh "Message via DNS-over-HTTPS"
./chimera_demo --transport dot "Message via DNS-over-TLS"
```

### Command Line Options

| Option | Description | Example |
|--------|-------------|---------|
| `--dns-server <ip>` | DNS server IP address | `--dns-server 8.8.8.8` |
| `--domain <domain>` | Target domain | `--domain example.com` |
| `--transport <type>` | Transport protocol | `--transport doh` |
| `--timeout <ms>` | Request timeout | `--timeout 10000` |
| `--help` | Show help message | `--help` |

## Core Concepts

### 1. Client Configuration

The `ClientConfig` structure controls all aspects of CHIMERA's behavior:

```cpp
chimera::ClientConfig config;

// Network settings
config.dns_server = "8.8.8.8";
config.dns_port = 53;
config.target_domain = "example.com";
config.timeout = std::chrono::milliseconds(5000);

// Transport selection
config.transport = chimera::TransportType::UDP;    // or DoH, DoT
config.use_random_subdomains = true;

// Security settings
config.use_hybrid_crypto = true;

// Behavioral mimicry (Phase 2)
config.adaptive_transport = true;
config.timing_variance = std::chrono::milliseconds(100);
config.behavioral_profile = chimera::BehavioralProfile::Normal;
```

### 2. Transport Types

CHIMERA supports three transport protocols:

#### UDP (Traditional DNS)
```cpp
config.transport = chimera::TransportType::UDP;
// Fast, low-latency, but visible as DNS traffic
```

#### DNS-over-HTTPS (DoH)
```cpp
config.transport = chimera::TransportType::DoH;
// Encrypted, harder to detect, uses HTTPS
```

#### DNS-over-TLS (DoT) 
```cpp
config.transport = chimera::TransportType::DoT;
// Encrypted, dedicated TLS connection
```

### 3. Message Sending

```cpp
chimera::ChimeraClient client(config);

// Simple text message
auto result = client.send_text("Secret message");

// Check result
if (result) {
    // Success
    std::cout << "Bytes sent: " << result->bytes_sent << std::endl;
    std::cout << "Latency: " << result->latency.count() << "ms" << std::endl;
    std::cout << "Domain used: " << result->used_domain << std::endl;
} else {
    // Handle error
    std::cerr << "Error occurred" << std::endl;
}
```

## Working Examples

### Example 1: Basic Message Sending

```cpp
#include "chimera/client.hpp"

void send_basic_message() {
    chimera::ClientConfig config;
    config.dns_server = "8.8.8.8";
    config.target_domain = "example.com";
    
    chimera::ChimeraClient client(config);
    
    std::string message = "This is a secret message!";
    auto result = client.send_text(message);
    
    if (result) {
        std::cout << "Message sent successfully!" << std::endl;
        std::cout << "Latency: " << result->latency.count() << "ms" << std::endl;
    }
}
```

### Example 2: Secure DoH Communication

```cpp
#include "chimera/client.hpp"

void send_secure_message() {
    chimera::ClientConfig config;
    config.dns_server = "1.1.1.1";              // Cloudflare
    config.target_domain = "your-domain.com";
    config.transport = chimera::TransportType::DoH;  // Use HTTPS
    config.timeout = std::chrono::milliseconds(10000);
    
    chimera::ChimeraClient client(config);
    
    std::string secure_message = "Confidential data transfer";
    auto result = client.send_text(secure_message);
    
    if (result) {
        std::cout << "Secure message sent via DoH!" << std::endl;
        std::cout << "Used domain: " << result->used_domain << std::endl;
    }
}
```

### Example 3: Multiple Messages with Error Handling

```cpp
#include "chimera/client.hpp"
#include <vector>

void send_multiple_messages() {
    chimera::ClientConfig config;
    config.server_ip = "8.8.8.8";
    config.target_domain = "example.com";
    config.use_random_subdomains = true;  // Use different subdomains
    
    chimera::ChimeraClient client(config);
    
    std::vector<std::string> messages = {
        "Message 1: System status OK",
        "Message 2: Initiating sequence",
        "Message 3: Operation complete"
    };
    
    for (const auto& message : messages) {
        auto result = client.send_text(message);
        
        if (result) {
            std::cout << "Sent: " << message << std::endl;
        } else {
            std::cerr << "Failed to send: " << message << std::endl;
            // Could implement retry logic here
        }
        
        // Add delay between messages
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
```

### Example 4: Configuration Testing

```cpp
#include "chimera/client.hpp"

void test_connectivity() {
    chimera::ClientConfig config;
    config.dns_server = "8.8.8.8";
    config.target_domain = "example.com";
    
    chimera::ChimeraClient client(config);
    
    // Test with ping first
    std::cout << "Testing connectivity..." << std::endl;
    auto ping_result = client.ping_dns_server();
    
    if (ping_result) {
        std::cout << "Connectivity OK, ping: " << ping_result->count() << "ms" << std::endl;
        auto result = client.send_text("Test message after ping");
        
        if (result) {
            std::cout << "Full test successful!" << std::endl;
        }
    } else {
        std::cout << "Connectivity issues detected" << std::endl;
    }
}
```

## Best Practices

### 1. Error Handling
Always check return values and handle errors gracefully:

```cpp
auto result = client.send_text(message);
if (!result) {
    // Log the error or retry
    std::cerr << "Send failed, retrying..." << std::endl;
}
```

### 2. Domain Selection
Use domains you control or public domains appropriately:

```cpp
// Good: Your own domain
config.target_domain = "my-service.com";

// Acceptable: Large public domain for testing
config.target_domain = "example.com";
```

### 3. Transport Selection
Choose transports based on your security needs:

```cpp
// High stealth: DoH (looks like web traffic)
config.transport = chimera::TransportType::DoH;

// Good security + performance: DoT
config.transport = chimera::TransportType::DoT;

// Fast, but visible: UDP
config.transport = chimera::TransportType::UDP;
```

### 4. Message Size Management
Keep messages reasonably sized for DNS:

```cpp
std::string message = "Keep messages under 200 characters for best performance";
// CHIMERA handles encoding automatically
```

## Common Patterns

### Configuration Factory Pattern
```cpp
chimera::ClientConfig create_secure_config() {
    chimera::ClientConfig config;
    config.dns_server = "1.1.1.1";
    config.transport = chimera::TransportType::DoH;
    config.use_hybrid_crypto = true;
    config.adaptive_transport = true;
    return config;
}
```

### Retry Logic
```cpp
bool send_with_retry(chimera::ChimeraClient& client, const std::string& message) {
    for (int i = 0; i < 3; ++i) {
        auto result = client.send_text(message);
        if (result) return true;
        
        std::this_thread::sleep_for(std::chrono::seconds(1 << i)); // Exponential backoff
    }
    return false;
}
```

## Next Steps

- **[Configuration](Configuration.md)** - Complete configuration reference
- **[Advanced Features](Advanced-Features.md)** - Phase 2 behavioral mimicry and async I/O
- **[Examples](Examples.md)** - More complex use cases and integration patterns

---

**Tip**: Start with UDP transport for development and testing, then switch to DoH or DoT for production deployments requiring stealth.