#include "chimera/client.hpp"
#include "chimera/base64.hpp"
#include "chimera/dns_packet.hpp"
#include "tl/expected.hpp"
#include <iostream>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <random>

namespace chimera {

tl::expected<SendResult, ChimeraError> ChimeraClient::send_text(const std::string& message) {
    auto start_time = std::chrono::steady_clock::now();

    // Socket létrehozás
    auto socket_result = create_udp_socket();
    if (!socket_result) {
        return tl::unexpected(socket_result.error());
    }

    int sock = socket_result.value();

    // Base64 kódolás
    std::string encoded_message;
    try {
        encoded_message = Base64::encode(message);
    } catch (const std::exception& e) {
        close(sock);
        std::cerr << "Base64 encoding error: " << e.what() << std::endl;
        return tl::unexpected(ChimeraError::EncodingError);
    }

    // Domain generálás
    std::string target_domain = config_.target_domain;
    if (config_.use_random_subdomains) {
        target_domain = generate_random_subdomain() + "." + config_.target_domain;
    }

    // DNS packet építés
    DnsQuestion question{target_domain, DnsType::TXT};
    std::vector<uint8_t> packet;
    try {
        packet = DnsPacketBuilder::build_query(question, encoded_message);
    } catch (const std::exception& e) {
        close(sock);
        std::cerr << "DNS packet building error: " << e.what() << std::endl;
        return tl::unexpected(ChimeraError::DnsError);
    }

    // Server cím beállítás
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config_.dns_port);
    if (inet_pton(AF_INET, config_.dns_server.c_str(), &server_addr.sin_addr) <= 0) {
        close(sock);
        std::cerr << "Invalid DNS server address: " << config_.dns_server << std::endl;
        return tl::unexpected(ChimeraError::ConfigError);
    }

    // Packet küldés
    ssize_t sent_bytes = sendto(sock, packet.data(), packet.size(), 0,
                               reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
    close(sock);

    if (sent_bytes < 0) {
        std::cerr << "Send error: " << strerror(errno) << std::endl;
        return tl::unexpected(ChimeraError::NetworkError);
    }

    auto end_time = std::chrono::steady_clock::now();
    auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // Debug információ
    std::cout << "Sent " << sent_bytes << " bytes in DNS packet" << std::endl;
    std::cout << "Used domain: " << target_domain << std::endl;
    std::cout << "Base64 payload: " << encoded_message << std::endl;
    std::cout << "Latency: " << latency.count() << "ms" << std::endl;

    return SendResult{
        .bytes_sent = static_cast<size_t>(sent_bytes),
        .latency = latency,
        .used_domain = target_domain
    };
}

tl::expected<std::chrono::milliseconds, ChimeraError> ChimeraClient::ping_dns_server() {
    auto start_time = std::chrono::steady_clock::now();

    // Egyszerű DNS query a ping-hez
    const DnsQuestion ping_question{"ping.test", DnsType::A};
    auto socket_result = create_udp_socket();
    if (!socket_result) {
        return tl::unexpected(socket_result.error());
    }

    int sock = socket_result.value();

    try {
        auto packet = DnsPacketBuilder::build_query(ping_question);

        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(config_.dns_port);
        inet_pton(AF_INET, config_.dns_server.c_str(), &server_addr.sin_addr);

        sendto(sock, packet.data(), packet.size(), 0,
               reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));

        // Válasz fogadás és parsing még nincs implementálva
        // Egyelőre csak a küldési időt mérjük
    } catch (const std::exception&) {
        close(sock);
        return tl::unexpected(ChimeraError::DnsError);
    }

    close(sock);
    const auto end_time = std::chrono::steady_clock::now();
    auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    return latency;
}

std::string ChimeraClient::generate_random_subdomain() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 35);

    // Random subdomain generálás
    std::string subdomain;
    constexpr char chars[] = "abcdefghijklmnopqrstuvwxyz0123456789";

    // 8-12 karakter hosszú subdomain
    std::uniform_int_distribution<> length_dis(8, 12);
    const int length = length_dis(gen);

    for (int i = 0; i < length; ++i) {
        subdomain += chars[dis(gen)];
    }

    return subdomain;
}

tl::expected<int, ChimeraError> ChimeraClient::create_udp_socket() const {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation error: " << strerror(errno) << std::endl;
        return tl::unexpected(ChimeraError::NetworkError);
    }

    // Timeout beállítás
    struct timeval tv{};
    tv.tv_sec = config_.timeout.count() / 1000;
    tv.tv_usec = (config_.timeout.count() % 1000) * 1000;

    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        close(sock);
        return tl::unexpected(ChimeraError::NetworkError);
    }

    return sock;
}

} // namespace chimera
