#pragma once

#include <string>
#include <expected>
#include <chrono>

// Client osztály, most már kicsit intelligensebb
// TODO: async I/O kellene, de egyelőre sync is jó

namespace chimera {

    enum class ChimeraError {
        NetworkError,
        ConfigError,
        EncodingError,
        TimeoutError,
        DnsError  // TODO: részletesebb DNS hibakódok
    };

    struct ClientConfig {
        std::string dns_server = "8.8.8.8";
        uint16_t dns_port = 53;
        std::string target_domain = "example.com";
        std::chrono::milliseconds timeout{5000};
        bool use_random_subdomains = true;  // TODO: implementálni

        // TODO: DoH/DoT beállítások
        // TODO: proxy beállítások
    };

    struct SendResult {
        size_t bytes_sent;
        std::chrono::milliseconds latency;
        std::string used_domain;  // melyik domain-t használtuk
    };

    class ChimeraClient {
        ClientConfig config_;

    public:
        explicit ChimeraClient(ClientConfig config) : config_(std::move(config)) {}

        // Szöveg küldés DNS TXT record-on keresztül
        std::expected<SendResult, ChimeraError> send_text(const std::string& message);

        // TODO: receive_text implementáció
        // TODO: file transfer support
        // TODO: interactive session mode

        // Konfiguráció lekérdezés/módosítás
        const ClientConfig& get_config() const { return config_; }
        void update_config(ClientConfig new_config) { config_ = std::move(new_config); }

        // Kapcsolat teszt
        std::expected<std::chrono::milliseconds, ChimeraError> ping_dns_server();

    private:
        static std::string generate_random_subdomain();
        std::expected<int, ChimeraError> create_udp_socket() const;

        // TODO: DoH client implementáció
        // TODO: DoT client implementáció
    };

} // namespace chimera
