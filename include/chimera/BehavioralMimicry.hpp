#pragma once

#include <chrono>
#include <random>
#include <vector>
#include <memory>
#include "common.hpp"

namespace chimera {

// Forward declaration
class ITransport;

struct TrafficPattern {
    std::chrono::milliseconds min_delay{100};
    std::chrono::milliseconds max_delay{2000};
    std::chrono::milliseconds burst_interval{5000};
    size_t max_burst_size = 3;
    double retry_probability = 0.1;
};

class BehavioralMimicry {
    BehavioralProfile profile_;
    TrafficPattern pattern_;
    mutable std::random_device rd_;
    mutable std::mt19937 gen_;
    mutable std::chrono::steady_clock::time_point last_request_;
    mutable size_t current_burst_count_ = 0;

public:
    explicit BehavioralMimicry(BehavioralProfile profile = BehavioralProfile::Normal);
    
    // Apply behavioral delays and patterns
    void apply_behavioral_delay() const;
    
    // Determine if transport should be switched
    bool should_switch_transport() const;
    
    // Get recommended transport for current conditions
    TransportType get_recommended_transport() const;
    
    // Update profile dynamically
    void set_profile(BehavioralProfile profile);
    
    // Get current pattern
    const TrafficPattern& get_pattern() const { return pattern_; }

private:
    void update_pattern();
    std::chrono::milliseconds calculate_delay() const;
    bool is_in_burst_window() const;
};

// Transport switching strategy for evasion
class AdaptiveTransportManager {
    std::vector<TransportType> available_transports_;
    mutable std::random_device rd_;
    mutable std::mt19937 gen_;
    mutable size_t current_transport_index_ = 0;
    mutable std::chrono::steady_clock::time_point last_switch_;
    std::chrono::milliseconds switch_interval_{30000}; // 30 seconds
    
public:
    AdaptiveTransportManager();
    
    // Add available transport types
    void add_transport(TransportType transport);
    
    // Get next transport (round-robin or random)
    TransportType get_next_transport(bool random = true) const;
    
    // Force transport switch
    void force_switch();
    
    // Check if it's time to switch
    bool should_switch() const;
    
    // Set switching interval
    void set_switch_interval(std::chrono::milliseconds interval) {
        switch_interval_ = interval;
    }
};

} // namespace chimera