# Advanced Features

This guide covers CHIMERA's Phase 2 advanced features including behavioral mimicry, async I/O, and adaptive transport management.

## Overview

Phase 2 introduces sophisticated evasion techniques and performance optimizations:

- **Behavioral Mimicry**: Traffic patterns that mimic legitimate DNS usage
- **Adaptive Transport Manager**: Dynamic protocol switching
- **Async I/O Framework**: High-performance asynchronous operations
- **Multi-Transport Support**: Seamless switching between UDP/DoH/DoT

## Behavioral Mimicry

### Concept

Behavioral mimicry makes CHIMERA traffic look like legitimate DNS usage by mimicking real user patterns:

```cpp
#include "chimera/BehavioralMimicry.hpp"

// Create behavioral mimicry instance
chimera::BehavioralMimicry mimicry(chimera::BehavioralProfile::WebBrowsing);

// Get timing delays that mimic web browsing
auto delay = mimicry.get_next_delay();
std::this_thread::sleep_for(delay);

// Get recommended transport for this profile
auto transport = mimicry.get_recommended_transport();
```

### Behavioral Profiles

#### Normal Profile
```cpp
chimera::BehavioralMimicry normal(BehavioralProfile::Normal);
```
- **Pattern**: Moderate, steady DNS requests
- **Timing**: 1-5 second intervals
- **Transport**: Balanced UDP/DoH usage
- **Use case**: General purpose, balanced stealth/performance

#### WebBrowsing Profile
```cpp
chimera::BehavioralMimicry web(BehavioralProfile::WebBrowsing);
```
- **Pattern**: Bursty requests followed by quiet periods
- **Timing**: Clusters of 3-10 requests, then 10-30 second gaps
- **Transport**: Prefers DoH (like browsers)
- **Use case**: Mimicking web browser DNS behavior

#### Enterprise Profile
```cpp
chimera::BehavioralMimicry enterprise(BehavioralProfile::Enterprise);
```
- **Pattern**: Regular, predictable intervals
- **Timing**: Consistent 30-60 second intervals
- **Transport**: Prefers DoT for corporate environments
- **Use case**: Automated systems, corporate networks

#### Gaming Profile
```cpp
chimera::BehavioralMimicry gaming(BehavioralProfile::Gaming);
```
- **Pattern**: Low-latency, frequent requests
- **Timing**: Sub-second intervals, minimal variance
- **Transport**: Prefers UDP for speed
- **Use case**: Real-time applications, low-latency requirements

#### Random Profile
```cpp
chimera::BehavioralMimicry random(BehavioralProfile::Random);
```
- **Pattern**: Unpredictable, highly varied
- **Timing**: Random intervals from 100ms to 60 seconds
- **Transport**: Random selection
- **Use case**: Maximum unpredictability, advanced evasion

### Using Behavioral Mimicry

#### Basic Usage
```cpp
#include "chimera/client.hpp"
#include "chimera/BehavioralMimicry.hpp"

void send_with_behavioral_mimicry() {
    // Configure client with behavioral profile
    chimera::ClientConfig config;
    config.server_ip = "1.1.1.1";
    config.target_domain = "example.com";
    config.behavioral_profile = BehavioralProfile::WebBrowsing;
    config.adaptive_transport = true;  // Enable transport switching
    
    chimera::ChimeraClient client(config);
    
    // Create behavioral mimicry manager
    chimera::BehavioralMimicry mimicry(BehavioralProfile::WebBrowsing);
    
    std::vector<std::string> messages = {
        "Message 1", "Message 2", "Message 3"
    };
    
    for (const auto& message : messages) {
        // Send message
        auto result = client.send_text(message);
        
        if (result) {
            std::cout << "Sent: " << message << std::endl;
        }
        
        // Wait using behavioral timing
        auto delay = mimicry.get_next_delay();
        std::cout << "Waiting " << delay.count() << "ms..." << std::endl;
        std::this_thread::sleep_for(delay);
    }
}
```

#### Advanced Pattern Control
```cpp
void advanced_behavioral_control() {
    chimera::BehavioralMimicry mimicry(BehavioralProfile::WebBrowsing);
    
    // Get detailed pattern information
    auto pattern = mimicry.get_pattern();
    std::cout << "Min delay: " << pattern.min_delay.count() << "ms" << std::endl;
    std::cout << "Max delay: " << pattern.max_delay.count() << "ms" << std::endl;
    std::cout << "Burst size: " << pattern.burst_size << std::endl;
    
    // Simulate burst pattern
    for (int burst = 0; burst < pattern.burst_size; ++burst) {
        // Send rapid messages (web browsing burst)
        send_message("Burst message " + std::to_string(burst));
        
        // Short delay within burst
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Wait for inter-burst delay
    auto inter_burst_delay = mimicry.get_inter_burst_delay();
    std::this_thread::sleep_for(inter_burst_delay);
}
```

## Adaptive Transport Manager

### Concept

The Adaptive Transport Manager automatically switches between UDP, DoH, and DoT based on:
- Behavioral profile requirements
- Network conditions
- Evasion needs

```cpp
#include "chimera/BehavioralMimicry.hpp"

// Create adaptive transport manager
chimera::AdaptiveTransportManager manager;

// Get next transport based on conditions
auto transport = manager.get_next_transport(false);  // Success = false (retry)
```

### Transport Selection Logic

#### Success-Based Adaptation
```cpp
void adaptive_transport_example() {
    chimera::AdaptiveTransportManager manager;
    chimera::ChimeraClient client(config);
    
    for (const auto& message : messages) {
        // Get recommended transport
        auto transport = manager.get_next_transport(last_success);
        
        // Update client configuration
        auto new_config = client.get_config();
        new_config.transport = transport;
        client.update_config(new_config);
        
        // Send message
        auto result = client.send_text(message);
        last_success = result.has_value();
        
        if (!last_success) {
            std::cout << "Failed with " << transport_name(transport) 
                      << ", trying different transport..." << std::endl;
        }
    }
}
```

#### Profile-Based Selection
```cpp
void profile_based_transport() {
    chimera::BehavioralMimicry web_mimicry(BehavioralProfile::WebBrowsing);
    chimera::BehavioralMimicry gaming_mimicry(BehavioralProfile::Gaming);
    
    // Web browsing prefers DoH
    auto web_transport = web_mimicry.get_recommended_transport();
    // Likely returns: TransportType::DoH
    
    // Gaming prefers UDP for speed
    auto gaming_transport = gaming_mimicry.get_recommended_transport();
    // Likely returns: TransportType::UDP
}
```

## Async I/O Framework

### Overview

CHIMERA's async I/O framework provides high-performance non-blocking operations:

```cpp
#include "chimera/AsyncIO.hpp"

// Create async I/O manager
chimera::AsyncIOManager io_manager;

// Create async client
chimera::AsyncChimeraClient async_client(config, io_manager);
```

### Async Operations

#### Future-Based API
```cpp
#include "chimera/AsyncIO.hpp"
#include <future>

void future_based_async() {
    chimera::AsyncIOManager io_manager;
    chimera::AsyncChimeraClient async_client(config, io_manager);
    
    // Start async operation
    auto future = async_client.send_text_async("Async message");
    
    // Do other work while message sends
    perform_other_tasks();
    
    // Wait for completion
    try {
        auto result = future.get();
        if (result) {
            std::cout << "Async send successful!" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Async send failed: " << e.what() << std::endl;
    }
}
```

#### Callback-Based API
```cpp
void callback_based_async() {
    chimera::AsyncIOManager io_manager;
    chimera::AsyncChimeraClient async_client(config, io_manager);
    
    // Send with callback
    async_client.send_text_async("Callback message", 
        [](const chimera::AsyncResult& result) {
            if (result.success) {
                std::cout << "Callback: Message sent!" << std::endl;
            } else {
                std::cout << "Callback: Send failed!" << std::endl;
            }
        });
    
    // Continue with other work
    // Callback will be called when operation completes
}
```

#### Concurrent Operations
```cpp
void concurrent_operations() {
    chimera::AsyncIOManager io_manager;
    chimera::AsyncChimeraClient async_client(config, io_manager);
    
    std::vector<std::future<chimera::SendResult>> futures;
    
    // Start multiple async operations
    for (int i = 0; i < 10; ++i) {
        std::string message = "Concurrent message " + std::to_string(i);
        futures.push_back(async_client.send_text_async(message));
    }
    
    // Wait for all to complete
    for (auto& future : futures) {
        try {
            auto result = future.get();
            if (result) {
                std::cout << "Message sent: " << result->message_sent << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Concurrent send failed: " << e.what() << std::endl;
        }
    }
}
```

### Performance Monitoring

```cpp
void performance_monitoring() {
    chimera::AsyncIOManager io_manager;
    
    // Start performance monitoring
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Run operations
    run_async_operations(io_manager);
    
    // Calculate performance metrics
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time);
    
    std::cout << "Operations completed in: " << duration.count() << "ms" << std::endl;
}
```

## Integration Patterns

### Complete Advanced Example
```cpp
#include "chimera/client.hpp"
#include "chimera/BehavioralMimicry.hpp"
#include "chimera/AsyncIO.hpp"

class AdvancedChimeraSession {
private:
    chimera::ClientConfig config_;
    chimera::BehavioralMimicry mimicry_;
    chimera::AdaptiveTransportManager transport_manager_;
    chimera::AsyncIOManager io_manager_;
    chimera::AsyncChimeraClient async_client_;
    
public:
    AdvancedChimeraSession(const chimera::ClientConfig& config)
        : config_(config)
        , mimicry_(config.behavioral_profile)
        , async_client_(config, io_manager_) {
    }
    
    void send_message_advanced(const std::string& message) {
        // Get behavioral timing
        auto delay = mimicry_.get_next_delay();
        
        // Get adaptive transport
        auto transport = transport_manager_.get_next_transport(true);
        
        // Update configuration
        config_.transport = transport;
        async_client_.update_config(config_);
        
        // Send with behavioral delay
        std::this_thread::sleep_for(delay);
        
        // Send async with callback
        async_client_.send_text_async(message,
            [this](const chimera::AsyncResult& result) {
                if (result.success) {
                    std::cout << "Advanced send successful" << std::endl;
                } else {
                    std::cout << "Advanced send failed, adapting..." << std::endl;
                    // Trigger transport adaptation
                    transport_manager_.get_next_transport(false);
                }
            });
    }
    
    void send_burst_messages(const std::vector<std::string>& messages) {
        auto pattern = mimicry_.get_pattern();
        
        for (size_t i = 0; i < messages.size(); ++i) {
            send_message_advanced(messages[i]);
            
            // Handle burst patterns
            if ((i + 1) % pattern.burst_size == 0) {
                auto inter_burst_delay = mimicry_.get_inter_burst_delay();
                std::this_thread::sleep_for(inter_burst_delay);
            }
        }
    }
};
```

### Usage of Advanced Session
```cpp
void use_advanced_session() {
    // Configure for high stealth
    chimera::ClientConfig config;
    config.server_ip = "1.1.1.1";
    config.target_domain = "example.com";
    config.behavioral_profile = BehavioralProfile::WebBrowsing;
    config.adaptive_transport = true;
    config.timing_variance = std::chrono::milliseconds(200);
    
    // Create advanced session
    AdvancedChimeraSession session(config);
    
    // Send messages with full behavioral mimicry
    std::vector<std::string> messages = {
        "Advanced message 1",
        "Advanced message 2", 
        "Advanced message 3"
    };
    
    session.send_burst_messages(messages);
}
```

## Performance Considerations

### Optimization Tips

1. **Transport Selection**:
   ```cpp
   // For maximum speed
   config.transport = TransportType::UDP;
   config.behavioral_profile = BehavioralProfile::Gaming;
   
   // For maximum stealth
   config.transport = TransportType::DoH;
   config.behavioral_profile = BehavioralProfile::Random;
   ```

2. **Async Operations**:
   ```cpp
   // Use async for concurrent operations
   auto future1 = async_client.send_text_async("Message 1");
   auto future2 = async_client.send_text_async("Message 2");
   
   // Both send concurrently
   auto result1 = future1.get();
   auto result2 = future2.get();
   ```

3. **Behavioral Tuning**:
   ```cpp
   // Reduce timing variance for better performance
   config.timing_variance = std::chrono::milliseconds(50);
   
   // Use Normal profile for balanced performance/stealth
   config.behavioral_profile = BehavioralProfile::Normal;
   ```

## Next Steps

- **[API Reference](API-Reference.md)** - Complete API documentation
- **[Examples](Examples.md)** - Real-world usage patterns
- **[Troubleshooting](Troubleshooting.md)** - Advanced debugging techniques

---

**Note**: Advanced features provide powerful evasion capabilities but may impact performance. Test thoroughly in your environment before production deployment.