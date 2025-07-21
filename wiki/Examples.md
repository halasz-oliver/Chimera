# Examples

Real-world examples for the CHIMERA framework using only implemented features.

## Basic Examples

### Simple Message Sending

```cpp
#include "chimera/client.hpp"
#include <iostream>

int main() {
    // Basic configuration
    chimera::ClientConfig config;
    config.dns_server = "8.8.8.8";
    config.target_domain = "example.com";
    
    // Create client and send message
    chimera::ChimeraClient client(config);
    auto result = client.send_text("Hello from CHIMERA!");
    
    if (result) {
        std::cout << "Message sent successfully!" << std::endl;
        std::cout << "Bytes sent: " << result->bytes_sent << std::endl;
        std::cout << "Latency: " << result->latency.count() << "ms" << std::endl;
        std::cout << "Domain: " << result->used_domain << std::endl;
    } else {
        std::cerr << "Failed to send message" << std::endl;
    }
    
    return 0;
}
```

### File-Based Messaging

```cpp
#include "chimera/client.hpp"
#include <fstream>
#include <sstream>

std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    chimera::ClientConfig config;
    config.dns_server = "8.8.8.8";
    config.target_domain = "example.com";
    
    chimera::ChimeraClient client(config);
    
    // Read message from file
    std::string message = read_file("secret_message.txt");
    
    auto result = client.send_text(message);
    if (result) {
        std::cout << "File content sent successfully!" << std::endl;
        std::cout << "Sent " << result->bytes_sent << " bytes" << std::endl;
    }
    
    return 0;
}
```

## Transport Examples

### Multi-Transport Fallback

```cpp
#include "chimera/client.hpp"

class ResilientMessenger {
private:
    std::vector<chimera::TransportType> transports_ = {
        chimera::TransportType::DoH,  // Try encrypted first
        chimera::TransportType::DoT,  // Fallback to DoT
        chimera::TransportType::UDP   // Last resort
    };
    
    chimera::ClientConfig base_config_;
    
public:
    ResilientMessenger() {
        base_config_.dns_server = "8.8.8.8";
        base_config_.target_domain = "example.com";
        base_config_.timeout = std::chrono::milliseconds(10000);
        base_config_.use_hybrid_crypto = true;
    }
    
    bool send_with_fallback(const std::string& message) {
        for (auto transport : transports_) {
            auto config = base_config_;
            config.transport = transport;
            
            chimera::ChimeraClient client(config);
            
            std::cout << "Trying " << transport_name(transport) << "..." << std::endl;
            
            auto result = client.send_text(message);
            if (result) {
                std::cout << "Success with " << transport_name(transport) << std::endl;
                std::cout << "Latency: " << result->latency.count() << "ms" << std::endl;
                return true;
            } else {
                std::cout << "Failed with " << transport_name(transport) 
                          << ", trying next..." << std::endl;
            }
        }
        
        std::cout << "All transports failed!" << std::endl;
        return false;
    }
    
private:
    std::string transport_name(chimera::TransportType transport) {
        switch (transport) {
            case chimera::TransportType::UDP: return "UDP";
            case chimera::TransportType::DoH: return "DoH";
            case chimera::TransportType::DoT: return "DoT";
            default: return "Unknown";
        }
    }
};

int main() {
    ResilientMessenger messenger;
    messenger.send_with_fallback("Critical message requiring delivery");
    return 0;
}
```

## Behavioral Mimicry Examples

### Using Behavioral Patterns

```cpp
#include "chimera/client.hpp"
#include "chimera/BehavioralMimicry.hpp"

void send_with_behavioral_mimicry() {
    // Configure client with behavioral profile
    chimera::ClientConfig config;
    config.dns_server = "1.1.1.1";
    config.target_domain = "example.com";
    config.behavioral_profile = chimera::BehavioralProfile::WebBrowsing;
    config.adaptive_transport = true;
    config.timing_variance = std::chrono::milliseconds(200);
    
    chimera::ChimeraClient client(config);
    
    // Create behavioral mimicry manager
    chimera::BehavioralMimicry mimicry(chimera::BehavioralProfile::WebBrowsing);
    
    std::vector<std::string> messages = {
        "Message 1", "Message 2", "Message 3"
    };
    
    for (const auto& message : messages) {
        // Apply behavioral delay
        mimicry.apply_behavioral_delay();
        
        // Send message
        auto result = client.send_text(message);
        
        if (result) {
            std::cout << "Sent: " << message << std::endl;
            std::cout << "Domain: " << result->used_domain << std::endl;
        }
    }
}
```

### Adaptive Transport Switching

```cpp
#include "chimera/BehavioralMimicry.hpp"

void adaptive_transport_example() {
    chimera::AdaptiveTransportManager manager;
    
    // Add available transports
    manager.add_transport(chimera::TransportType::UDP);
    manager.add_transport(chimera::TransportType::DoH);
    manager.add_transport(chimera::TransportType::DoT);
    
    chimera::ClientConfig config;
    config.dns_server = "8.8.8.8";
    config.target_domain = "example.com";
    
    std::vector<std::string> messages = {"Message 1", "Message 2", "Message 3"};
    bool last_success = true;
    
    for (const auto& message : messages) {
        // Get recommended transport
        auto transport = manager.get_next_transport(true); // Random selection
        
        // Update client configuration
        config.transport = transport;
        chimera::ChimeraClient client(config);
        
        // Send message
        auto result = client.send_text(message);
        last_success = result.has_value();
        
        if (!last_success) {
            std::cout << "Failed with transport, switching..." << std::endl;
            manager.force_switch();
        } else {
            std::cout << "Message sent successfully" << std::endl;
        }
    }
}
```

## Performance Examples

### Connectivity Testing

```cpp
#include "chimera/client.hpp"
#include <chrono>

class ConnectivityTester {
private:
    chimera::ChimeraClient client_;
    
public:
    ConnectivityTester(const chimera::ClientConfig& config) : client_(config) {}
    
    void test_performance() {
        std::cout << "Testing DNS server connectivity..." << std::endl;
        
        // Ping test
        auto ping_result = client_.ping_dns_server();
        if (ping_result) {
            std::cout << "Ping successful: " << ping_result->count() << "ms" << std::endl;
        } else {
            std::cout << "Ping failed" << std::endl;
            return;
        }
        
        // Message sending test
        auto start = std::chrono::high_resolution_clock::now();
        auto result = client_.send_text("Performance test message");
        auto end = std::chrono::high_resolution_clock::now();
        
        if (result) {
            auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << "Message sent successfully!" << std::endl;
            std::cout << "Network latency: " << result->latency.count() << "ms" << std::endl;
            std::cout << "Total time: " << total_time.count() << "ms" << std::endl;
            std::cout << "Bytes sent: " << result->bytes_sent << std::endl;
        } else {
            std::cout << "Message send failed" << std::endl;
        }
    }
};

int main() {
    chimera::ClientConfig config;
    config.dns_server = "8.8.8.8";
    config.target_domain = "example.com";
    config.transport = chimera::TransportType::UDP;  // Fastest transport
    
    ConnectivityTester tester(config);
    tester.test_performance();
    
    return 0;
}
```

## Configuration Examples

### Different Transport Configurations

```cpp
#include "chimera/client.hpp"

chimera::ClientConfig create_udp_config() {
    chimera::ClientConfig config;
    config.dns_server = "8.8.8.8";
    config.target_domain = "example.com";
    config.transport = chimera::TransportType::UDP;
    config.timeout = std::chrono::milliseconds(3000);
    config.use_random_subdomains = true;
    return config;
}

chimera::ClientConfig create_doh_config() {
    chimera::ClientConfig config;
    config.dns_server = "1.1.1.1";  // Cloudflare supports DoH
    config.target_domain = "example.com";
    config.transport = chimera::TransportType::DoH;
    config.timeout = std::chrono::milliseconds(10000);
    config.use_hybrid_crypto = true;
    return config;
}

chimera::ClientConfig create_dot_config() {
    chimera::ClientConfig config;
    config.dns_server = "9.9.9.9";  // Quad9 supports DoT
    config.target_domain = "example.com";
    config.transport = chimera::TransportType::DoT;
    config.timeout = std::chrono::milliseconds(8000);
    config.use_hybrid_crypto = true;
    return config;
}

void test_all_transports() {
    std::string test_message = "Transport test message";
    
    // Test UDP
    {
        auto config = create_udp_config();
        chimera::ChimeraClient client(config);
        auto result = client.send_text(test_message);
        std::cout << "UDP: " << (result ? "SUCCESS" : "FAILED") << std::endl;
    }
    
    // Test DoH
    {
        auto config = create_doh_config();
        chimera::ChimeraClient client(config);
        auto result = client.send_text(test_message);
        std::cout << "DoH: " << (result ? "SUCCESS" : "FAILED") << std::endl;
    }
    
    // Test DoT
    {
        auto config = create_dot_config();
        chimera::ChimeraClient client(config);
        auto result = client.send_text(test_message);
        std::cout << "DoT: " << (result ? "SUCCESS" : "FAILED") << std::endl;
    }
}
```

## Error Handling Examples

### Comprehensive Error Handling

```cpp
#include "chimera/client.hpp"

void robust_message_sending() {
    chimera::ClientConfig config;
    config.dns_server = "8.8.8.8";
    config.target_domain = "example.com";
    config.timeout = std::chrono::milliseconds(5000);
    
    chimera::ChimeraClient client(config);
    
    std::string message = "Important message";
    
    // Test connectivity first
    auto ping_result = client.ping_dns_server();
    if (!ping_result) {
        std::cerr << "DNS server unreachable, aborting" << std::endl;
        return;
    }
    
    std::cout << "DNS server responding (ping: " << ping_result->count() << "ms)" << std::endl;
    
    // Attempt to send message with retry logic
    const int max_retries = 3;
    for (int attempt = 1; attempt <= max_retries; ++attempt) {
        std::cout << "Attempt " << attempt << "/" << max_retries << "..." << std::endl;
        
        auto result = client.send_text(message);
        
        if (result) {
            std::cout << "Success on attempt " << attempt << "!" << std::endl;
            std::cout << "Bytes sent: " << result->bytes_sent << std::endl;
            std::cout << "Latency: " << result->latency.count() << "ms" << std::endl;
            std::cout << "Used domain: " << result->used_domain << std::endl;
            return;
        } else {
            std::cout << "Attempt " << attempt << " failed" << std::endl;
            if (attempt < max_retries) {
                std::cout << "Retrying in 2 seconds..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }
    }
    
    std::cerr << "All attempts failed, message not sent" << std::endl;
}
```

## Next Steps

- **[Troubleshooting](Troubleshooting.md)** - Debug common issues
- **[FAQ](FAQ.md)** - Frequently asked questions
- **[API Reference](API-Reference.md)** - Complete API documentation

---

**Note**: These examples use only the currently implemented CHIMERA features. All code is tested and functional.