#pragma once

#include <string>
#include <chrono>
#include "tl/expected.hpp"

// Kliens osztály - DNS steganográfia kezelés
// Most már hibrid kulcscserével is
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
        bool use_hybrid_crypto = true;  // Hibrid kulcscsere engedélyezés
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

        // Szöveg küldés DNS TXT record-on keresztül
        tl::expected<SendResult, ChimeraError> send_text(const std::string& message);

        // Konfiguráció lekérdezés/módosítás
        [[nodiscard]] const ClientConfig& get_config() const { return config_; }
        void update_config(ClientConfig new_config) { config_ = std::move(new_config); }

        // Kapcsolat teszt
        tl::expected<std::chrono::milliseconds, ChimeraError> ping_dns_server();

    private:
        static std::string generate_random_subdomain();
        [[nodiscard]] tl::expected<int, ChimeraError> create_udp_socket() const;
    };

} // namespace chimera
