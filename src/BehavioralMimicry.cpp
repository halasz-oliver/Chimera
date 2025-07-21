#include "chimera/BehavioralMimicry.hpp"
#include <thread>
#include <algorithm>

namespace chimera {

BehavioralMimicry::BehavioralMimicry(BehavioralProfile profile)
    : profile_(profile), gen_(rd_()) {
    update_pattern();
    last_request_ = std::chrono::steady_clock::now();
}

void BehavioralMimicry::apply_behavioral_delay() const {
    auto delay = calculate_delay();
    std::this_thread::sleep_for(delay);
    last_request_ = std::chrono::steady_clock::now();
}

bool BehavioralMimicry::should_switch_transport() const {
    // Implement transport switching logic based on patterns
    std::uniform_real_distribution<> dis(0.0, 1.0);
    double switch_probability = 0.0;
    
    switch (profile_) {
        case BehavioralProfile::Normal:
            switch_probability = 0.01; // 1% chance
            break;
        case BehavioralProfile::WebBrowsing:
            switch_probability = 0.05; // 5% chance (more dynamic)
            break;
        case BehavioralProfile::Enterprise:
            switch_probability = 0.02; // 2% chance (conservative)
            break;
        case BehavioralProfile::Gaming:
            switch_probability = 0.03; // 3% chance
            break;
        case BehavioralProfile::Random:
            switch_probability = 0.1; // 10% chance (very dynamic)
            break;
    }
    
    return dis(gen_) < switch_probability;
}

TransportType BehavioralMimicry::get_recommended_transport() const {
    std::uniform_int_distribution<> transport_dis(0, 2);
    
    switch (profile_) {
        case BehavioralProfile::Normal:
            return TransportType::UDP; // Traditional DNS
        case BehavioralProfile::WebBrowsing:
            return TransportType::DoH; // HTTPS-based
        case BehavioralProfile::Enterprise:
            return transport_dis(gen_) == 0 ? TransportType::DoT : TransportType::UDP;
        case BehavioralProfile::Gaming:
            return TransportType::UDP; // Low latency
        case BehavioralProfile::Random:
            return static_cast<TransportType>(transport_dis(gen_));
    }
    return TransportType::UDP;
}

void BehavioralMimicry::set_profile(BehavioralProfile profile) {
    profile_ = profile;
    update_pattern();
}

void BehavioralMimicry::update_pattern() {
    switch (profile_) {
        case BehavioralProfile::Normal:
            pattern_ = {
                .min_delay = std::chrono::milliseconds(200),
                .max_delay = std::chrono::milliseconds(1000),
                .burst_interval = std::chrono::milliseconds(10000),
                .max_burst_size = 2,
                .retry_probability = 0.05
            };
            break;
        case BehavioralProfile::WebBrowsing:
            pattern_ = {
                .min_delay = std::chrono::milliseconds(50),
                .max_delay = std::chrono::milliseconds(3000),
                .burst_interval = std::chrono::milliseconds(2000),
                .max_burst_size = 5,
                .retry_probability = 0.15
            };
            break;
        case BehavioralProfile::Enterprise:
            pattern_ = {
                .min_delay = std::chrono::milliseconds(500),
                .max_delay = std::chrono::milliseconds(2000),
                .burst_interval = std::chrono::milliseconds(15000),
                .max_burst_size = 3,
                .retry_probability = 0.08
            };
            break;
        case BehavioralProfile::Gaming:
            pattern_ = {
                .min_delay = std::chrono::milliseconds(10),
                .max_delay = std::chrono::milliseconds(100),
                .burst_interval = std::chrono::milliseconds(1000),
                .max_burst_size = 8,
                .retry_probability = 0.2
            };
            break;
        case BehavioralProfile::Random:
            pattern_ = {
                .min_delay = std::chrono::milliseconds(10),
                .max_delay = std::chrono::milliseconds(5000),
                .burst_interval = std::chrono::milliseconds(1000),
                .max_burst_size = 10,
                .retry_probability = 0.25
            };
            break;
    }
}

std::chrono::milliseconds BehavioralMimicry::calculate_delay() const {
    std::uniform_int_distribution<> delay_dis(
        static_cast<int>(pattern_.min_delay.count()),
        static_cast<int>(pattern_.max_delay.count())
    );
    
    auto base_delay = std::chrono::milliseconds(delay_dis(gen_));
    
    // Apply burst logic
    if (is_in_burst_window() && current_burst_count_ < pattern_.max_burst_size) {
        base_delay /= 2; // Reduce delay during bursts
        current_burst_count_++;
    } else {
        current_burst_count_ = 0;
    }
    
    return base_delay;
}

bool BehavioralMimicry::is_in_burst_window() const {
    auto now = std::chrono::steady_clock::now();
    auto time_since_last = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_request_);
    return time_since_last < pattern_.burst_interval;
}

// AdaptiveTransportManager implementation
AdaptiveTransportManager::AdaptiveTransportManager() : gen_(rd_()) {
    // Add all transport types by default
    available_transports_ = {TransportType::UDP, TransportType::DoH, TransportType::DoT};
    last_switch_ = std::chrono::steady_clock::now();
}

void AdaptiveTransportManager::add_transport(TransportType transport) {
    if (std::find(available_transports_.begin(), available_transports_.end(), transport) 
        == available_transports_.end()) {
        available_transports_.push_back(transport);
    }
}

TransportType AdaptiveTransportManager::get_next_transport(bool random) const {
    if (available_transports_.empty()) {
        return TransportType::UDP; // Fallback
    }
    
    if (random) {
        std::uniform_int_distribution<size_t> dis(0, available_transports_.size() - 1);
        return available_transports_[dis(gen_)];
    } else {
        // Round-robin
        current_transport_index_ = (current_transport_index_ + 1) % available_transports_.size();
        return available_transports_[current_transport_index_];
    }
}

void AdaptiveTransportManager::force_switch() {
    last_switch_ = std::chrono::steady_clock::now();
}

bool AdaptiveTransportManager::should_switch() const {
    auto now = std::chrono::steady_clock::now();
    auto time_since_switch = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_switch_);
    return time_since_switch >= switch_interval_;
}

} // namespace chimera