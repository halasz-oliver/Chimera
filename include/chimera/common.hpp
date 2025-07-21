#pragma once

namespace chimera {

enum class TransportType {
    UDP,
    DoH,
    DoT
};

enum class BehavioralProfile {
    Normal,        // Regular DNS queries
    WebBrowsing,   // Web browsing patterns  
    Enterprise,    // Corporate network patterns
    Gaming,        // Gaming traffic patterns
    Random         // Random pattern selection
};

} // namespace chimera