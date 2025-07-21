#pragma once

#include <string>
#include <chrono>
#include <cstdint>
#include <memory>
#include "tl/expected.hpp"
#include "common.hpp"

// Forward declarations
namespace chimera {
    class ITransport;
}

// Client class - DNS steganography handling
// Now also with hybrid key exchange
namespace chimera {

    enum class ChimeraError {
        NetworkError,
        ConfigError,
        EncodingError,
        TimeoutError,
        DnsError,
        CryptoError
    };

    struct ClientConfig {
        std::string dns_server = "8.8.8.8";
        uint16_t dns_port = 53;
        std::string target_domain = "example.com";
        std::chrono::milliseconds timeout{5000};
        bool use_random_subdomains = true;
        bool use_hybrid_crypto = true; // Enable hybrid key exchange
        TransportType transport = TransportType::UDP;
        bool adaptive_transport = false; // Behavioral mimicry
        std::chrono::milliseconds timing_variance{100}; // Jitter for behavioral mimicry
        BehavioralProfile behavioral_profile = BehavioralProfile::Normal;
    };

    struct SendResult {
        size_t bytes_sent;
        std::chrono::milliseconds latency;
        std::string used_domain;
    };

    class ChimeraClient {
        ClientConfig config_;

    public:
        explicit ChimeraClient(ClientConfig config) : config_(std::move(config)) {}

        // Text sending via DNS TXT record
        tl::expected<SendResult, ChimeraError> send_text(const std::string& message) const;

        // Configuration query/modification
        [[nodiscard]] const ClientConfig& get_config() const { return config_; }
        void update_config(ClientConfig new_config) { config_ = std::move(new_config); }

        // Kapcsolat teszt
        tl::expected<std::chrono::milliseconds, ChimeraError> ping_dns_server() const;

    private:
        std::unique_ptr<ITransport> create_transport() const;
        static std::string generate_random_subdomain();
        [[nodiscard]] tl::expected<int, ChimeraError> create_udp_socket() const;
    };

} // namespace chimera
