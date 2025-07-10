#include "chimera/client.hpp"
#include "chimera/base64.hpp"
#include "chimera/dns_packet.hpp"
#include "chimera/Transport.hpp"
#include <iostream>
#include <random>

namespace chimera {

tl::expected<SendResult, ChimeraError> ChimeraClient::send_text(const std::string& message) const {
    auto start_time = std::chrono::steady_clock::now();

    TransportUdp transport(config_.dns_server, config_.dns_port);
    transport.set_timeout(config_.timeout);

    // Base64 kódolás
    std::string encoded_message;
    try {
        encoded_message = Base64::encode(message);
    } catch (const std::exception& e) {
        std::cerr << "Base64 encoding error: " << e.what() << std::endl;
        return tl::unexpected(ChimeraError::EncodingError);
    }

    // Domain generálás
    std::string target_domain = config_.target_domain;
    if (config_.use_random_subdomains) {
        target_domain = generate_random_subdomain() + "." + config_.target_domain;
    }

    DnsQuestion question{target_domain, DnsType::TXT};
    std::vector<uint8_t> packet;
    try {
        packet = DnsPacketBuilder::build_query(question, encoded_message);
    } catch (const std::exception& e) {
        std::cerr << "DNS packet building error: " << e.what() << std::endl;
        return tl::unexpected(ChimeraError::DnsError);
    }

    auto send_result = transport.send(packet);
    if (!send_result) {
        std::cerr << "Send error" << std::endl;
        return tl::unexpected(ChimeraError::NetworkError);
    }

    auto end_time = std::chrono::steady_clock::now();
    auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "Sent bytes: " << send_result.value() << std::endl;
    std::cout << "Used domain: " << target_domain << std::endl;
    std::cout << "Latency: " << latency.count() << " ms" << std::endl;

    return SendResult{
        .bytes_sent = send_result.value(),
        .latency = latency,
        .used_domain = target_domain
    };
}

tl::expected<std::chrono::milliseconds, ChimeraError> ChimeraClient::ping_dns_server() const {
    const auto start_time = std::chrono::steady_clock::now();

    TransportUdp transport(config_.dns_server, config_.dns_port);
    transport.set_timeout(config_.timeout);

    const DnsQuestion ping_question{"ping.test", DnsType::A};
    std::vector<uint8_t> packet;
    try {
        packet = DnsPacketBuilder::build_query(ping_question);
    } catch (const std::exception& e) {
        std::cerr << "DNS packet building error for ping: " << e.what() << std::endl;
        return tl::unexpected(ChimeraError::DnsError);
    }

    auto send_result = transport.send(packet);
    if (!send_result) {
        std::cerr << "Ping send error" << std::endl;
        return tl::unexpected(ChimeraError::NetworkError);
    }

    auto recv_result = transport.receive();
    if (!recv_result) {
        std::cerr << "Ping receive error" << std::endl;
        return tl::unexpected(ChimeraError::NetworkError);
    }

    // Válasz feldolgozás
    std::vector<DnsResourceRecord> answers;
    try {
        DnsPacketBuilder::parse_response(recv_result.value(), answers);
    } catch (const std::exception& e) {
        std::cerr << "DNS response parsing error: " << e.what() << std::endl;
    }

    const auto end_time = std::chrono::steady_clock::now();
    auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "Ping response size: " << recv_result.value().size() << " bytes" << std::endl;
    std::cout << "Answer count: " << answers.size() << std::endl;
    std::cout << "Ping latency: " << latency.count() << " ms" << std::endl;

    return latency;
}

std::string ChimeraClient::generate_random_subdomain() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 35);
    std::string subdomain;
    constexpr char chars[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    std::uniform_int_distribution<> length_dis(8, 12);
    const int length = length_dis(gen);
    for (int i = 0; i < length; ++i) {
        subdomain += chars[dis(gen)];
    }
    return subdomain;
}

} // namespace chimera
