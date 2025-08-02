#pragma once

#include <string>
#include <chrono>
#include <cstdint>
#include <memory>
#include "tl/expected.hpp"
#include "common.hpp"
#include "steganography.hpp"

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
        DecodingError,
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
        
        // Phase 3: Steganographic Enhancement Configuration
        EncodingStrategy encoding_strategy = EncodingStrategy::MULTI_RECORD;
        bool use_compression = true;
        bool randomize_fragments = true;
        double noise_ratio = 0.1;
        size_t max_fragments = 10;
    };

    struct SendResult {
        size_t bytes_sent;
        std::chrono::milliseconds latency;
        std::string used_domain;
        
        // Phase 3: Enhanced result information
        std::vector<DnsType> used_record_types;
        size_t fragments_sent;
        EncodingStrategy encoding_used;
        bool compression_used;
    };

    class ChimeraClient {
        ClientConfig config_;

    public:
        explicit ChimeraClient(ClientConfig config) : config_(std::move(config)) {}

        // Text sending via DNS TXT record
        tl::expected<SendResult, ChimeraError> send_text(const std::string& message) const;

        // Phase 3: Enhanced steganographic sending methods
        tl::expected<SendResult, ChimeraError> send_data(const std::vector<uint8_t>& data) const;
        tl::expected<SendResult, ChimeraError> send_file(const std::string& file_path) const;
        
        // Phase 3: Multi-record steganographic sending
        tl::expected<SendResult, ChimeraError> send_multi_record(const std::vector<uint8_t>& data) const;
        
        // Phase 3: Bidirectional communication
        tl::expected<std::vector<uint8_t>, ChimeraError> receive_data(const std::string& query_domain) const;
        
        // Phase 3: Capacity estimation
        size_t estimate_capacity() const;

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
