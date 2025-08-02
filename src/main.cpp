#include "chimera/client.hpp"
#include "chimera/steganography.hpp"
#include <iostream>
#include <fstream>
#include <string>

// Simple CLI parsing - later replace with a proper library
void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options] [message_file]\n";
    std::cout << "Options:\n";
    std::cout << "  -s, --server    DNS server address (default: 8.8.8.8)\n";
    std::cout << "  -p, --port      DNS server port (default: 53)\n";
    std::cout << "  -d, --domain    Target domain (default: example.com)\n";
    std::cout << "  -t, --timeout   Timeout in milliseconds (default: 5000)\n";
    std::cout << "  --encoding      Encoding strategy: txt, multi, distributed (default: multi)\n";
    std::cout << "  --compress      Enable compression (default: true)\n";
    std::cout << "  --noise         Noise ratio 0.0-1.0 (default: 0.1)\n";
    std::cout << "  --demo-phase3   Run Phase 3 demonstration\n";
    std::cout << "  -h, --help      Show this help\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << program_name << " message.txt\n";
    std::cout << "  " << program_name << " -s 1.1.1.1 -d test.com --encoding multi message.txt\n";
    std::cout << "  " << program_name << " --demo-phase3\n";
}

void demo_phase3() {
    std::cout << "=== CHIMERA Phase 3 Demonstration ===" << std::endl;
    std::cout << "Steganographic Enhancement Features" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // Test different encoding strategies
    std::vector<std::pair<chimera::EncodingStrategy, std::string>> strategies = {
        {chimera::EncodingStrategy::TXT_ONLY, "TXT-only (Phase 1/2 compatible)"},
        {chimera::EncodingStrategy::MULTI_RECORD, "Multi-record (A, AAAA, TXT)"},
        {chimera::EncodingStrategy::DISTRIBUTED, "Distributed payload"}
    };
    
    std::string demo_message = "CHIMERA Phase 3: Advanced steganographic encoding with multi-record DNS support, "
                              "IPv4/IPv6 address encoding, enhanced TXT records, and HTTP/2 body encoding for "
                              "maximum steganographic capacity and detection evasion!";
    
    for (const auto& [strategy, description] : strategies) {
        std::cout << "\n--- " << description << " ---" << std::endl;
        
        chimera::ClientConfig config;
        config.target_domain = "demo.chimera-test.com";
        config.encoding_strategy = strategy;
        config.use_compression = true;
        config.randomize_fragments = false; // Deterministic for demo
        config.noise_ratio = 0.1;
        config.max_fragments = 8;
        
        chimera::ChimeraClient client(config);
        
        // Show capacity estimation
        auto capacity = client.estimate_capacity();
        std::cout << "Estimated capacity: " << capacity << " bytes" << std::endl;
        
        // Convert message to binary data
        std::vector<uint8_t> data(demo_message.begin(), demo_message.end());
        
        // Create encoder to show fragment breakdown
        chimera::EncodingConfig enc_config;
        enc_config.strategy = strategy;
        enc_config.use_compression = true;
        enc_config.randomize_order = false;
        enc_config.noise_ratio = 0.1;
        enc_config.max_fragments = 8;
        
        chimera::SteganographicEncoder encoder(enc_config);
        auto fragments_result = encoder.encode_payload(data, config.target_domain);
        
        if (fragments_result) {
            auto fragments = fragments_result.value();
            std::cout << "Generated " << fragments.size() << " fragments:" << std::endl;
            
            std::map<chimera::DnsType, int> type_counts;
            for (const auto& frag : fragments) {
                type_counts[frag.record_type]++;
            }
            
            for (const auto& [type, count] : type_counts) {
                std::string type_name;
                switch (type) {
                    case chimera::DnsType::A: type_name = "A (IPv4)"; break;
                    case chimera::DnsType::AAAA: type_name = "AAAA (IPv6)"; break;
                    case chimera::DnsType::TXT: type_name = "TXT"; break;
                    default: type_name = "Other"; break;
                }
                std::cout << "  " << type_name << ": " << count << " records" << std::endl;
            }
            
            // Show example domains
            std::cout << "Example domains:" << std::endl;
            for (size_t i = 0; i < std::min(size_t(3), fragments.size()); ++i) {
                std::cout << "  " << fragments[i].domain << std::endl;
            }
        }
    }
    
    std::cout << "\n--- Capacity Comparison ---" << std::endl;
    chimera::EncodingConfig config;
    config.max_fragments = 10;
    
    auto a_capacity = chimera::SteganographicEncoder::estimate_capacity(chimera::DnsType::A, 10);
    auto aaaa_capacity = chimera::SteganographicEncoder::estimate_capacity(chimera::DnsType::AAAA, 10);
    auto txt_capacity = chimera::SteganographicEncoder::estimate_capacity(chimera::DnsType::TXT, 10);
    
    std::cout << "A records (IPv4):     " << a_capacity << " bytes" << std::endl;
    std::cout << "AAAA records (IPv6):  " << aaaa_capacity << " bytes" << std::endl;
    std::cout << "TXT records:          " << txt_capacity << " bytes" << std::endl;
    
    config.strategy = chimera::EncodingStrategy::MULTI_RECORD;
    auto multi_capacity = chimera::SteganographicEncoder::estimate_total_capacity(config);
    std::cout << "Multi-record total:   " << multi_capacity << " bytes" << std::endl;
    
    std::cout << "\n--- IPv4/IPv6 Encoding Demo ---" << std::endl;
    std::vector<uint8_t> sample_data = {0xDE, 0xAD, 0xBE, 0xEF};
    
    auto ipv4_encoded = chimera::IPv4Encoding::encode_to_ipv4(sample_data, 0);
    std::cout << "IPv4 encoded: ";
    for (size_t i = 0; i < ipv4_encoded.size(); ++i) {
        if (i > 0) std::cout << ".";
        std::cout << static_cast<int>(ipv4_encoded[i]);
    }
    std::cout << std::endl;
    
    std::vector<uint8_t> ipv6_sample = {
        0xfe, 0x80, 0x00, 0x00, 0xde, 0xad, 0xbe, 0xef,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
    };
    auto ipv6_encoded = chimera::IPv6Encoding::encode_to_ipv6(ipv6_sample, 0);
    std::cout << "IPv6 encoded: ";
    for (size_t i = 0; i < ipv6_encoded.size(); i += 2) {
        if (i > 0) std::cout << ":";
        std::cout << std::hex << (ipv6_encoded[i] << 8 | ipv6_encoded[i+1]);
    }
    std::cout << std::dec << std::endl;
    
    std::cout << "\n=== Phase 3 Demonstration Complete ===" << std::endl;
}

int main(int argc, char** argv) {
    chimera::ClientConfig config;
    std::string message = "Hello from Chimera! 🦎";
    bool run_phase3_demo = false;

    // Basic argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            print_usage(argv[0]);
            return 0;
        } else if (arg == "--demo-phase3") {
            run_phase3_demo = true;
        } else if (arg == "-s" || arg == "--server") {
            if (i + 1 < argc) {
                config.dns_server = argv[++i];
            } else {
                std::cerr << "Missing DNS server address\n";
                return 1;
            }
        } else if (arg == "-p" || arg == "--port") {
            if (i + 1 < argc) {
                config.dns_port = static_cast<uint16_t>(std::stoi(argv[++i]));
            } else {
                std::cerr << "Missing port number\n";
                return 1;
            }
        } else if (arg == "-d" || arg == "--domain") {
            if (i + 1 < argc) {
                config.target_domain = argv[++i];
            } else {
                std::cerr << "Missing target domain\n";
                return 1;
            }
        } else if (arg == "-t" || arg == "--timeout") {
            if (i + 1 < argc) {
                config.timeout = std::chrono::milliseconds(std::stoi(argv[++i]));
            } else {
                std::cerr << "Missing timeout value\n";
                return 1;
            }
        } else if (arg == "--encoding") {
            if (i + 1 < argc) {
                std::string strategy = argv[++i];
                if (strategy == "txt") {
                    config.encoding_strategy = chimera::EncodingStrategy::TXT_ONLY;
                } else if (strategy == "multi") {
                    config.encoding_strategy = chimera::EncodingStrategy::MULTI_RECORD;
                } else if (strategy == "distributed") {
                    config.encoding_strategy = chimera::EncodingStrategy::DISTRIBUTED;
                } else {
                    std::cerr << "Invalid encoding strategy: " << strategy << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "Missing encoding strategy\n";
                return 1;
            }
        } else if (arg == "--compress") {
            config.use_compression = true;
        } else if (arg == "--noise") {
            if (i + 1 < argc) {
                config.noise_ratio = std::stod(argv[++i]);
                if (config.noise_ratio < 0.0 || config.noise_ratio > 1.0) {
                    std::cerr << "Noise ratio must be between 0.0 and 1.0\n";
                    return 1;
                }
            } else {
                std::cerr << "Missing noise ratio\n";
                return 1;
            }
        } else if (arg[0] != '-') {
            // Filename
            std::ifstream file(arg);
            if (file.is_open()) {
                std::string line;
                message.clear();
                while (std::getline(file, line)) {
                    if (!message.empty()) message += "\n";
                    message += line;
                }
                file.close();
                std::cout << "Message loaded from file: " << arg << std::endl;
            } else {
                std::cerr << "Could not open file: " << arg << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            print_usage(argv[0]);
            return 1;
        }
    }

    // Run Phase 3 demonstration if requested
    if (run_phase3_demo) {
        demo_phase3();
        return 0;
    }

    std::cout << "=== CHIMERA DNS Steganography Client ===" << std::endl;
    std::cout << "DNS Server: " << config.dns_server << ":" << config.dns_port << std::endl;
    std::cout << "Target Domain: " << config.target_domain << std::endl;
    std::cout << "Timeout: " << config.timeout.count() << "ms" << std::endl;
    std::cout << "Message length: " << message.size() << " characters" << std::endl;
    
    // Phase 3: Show encoding configuration
    std::cout << "--- Phase 3 Configuration ---" << std::endl;
    std::string strategy_name;
    switch (config.encoding_strategy) {
        case chimera::EncodingStrategy::TXT_ONLY: strategy_name = "TXT-only"; break;
        case chimera::EncodingStrategy::MULTI_RECORD: strategy_name = "Multi-record"; break;
        case chimera::EncodingStrategy::DISTRIBUTED: strategy_name = "Distributed"; break;
        case chimera::EncodingStrategy::HTTP2_BODY: strategy_name = "HTTP/2 body"; break;
    }
    std::cout << "Encoding strategy: " << strategy_name << std::endl;
    std::cout << "Compression: " << (config.use_compression ? "enabled" : "disabled") << std::endl;
    std::cout << "Noise ratio: " << config.noise_ratio << std::endl;
    std::cout << "Max fragments: " << config.max_fragments << std::endl;
    
    chimera::ChimeraClient client(config);
    auto capacity = client.estimate_capacity();
    std::cout << "Estimated capacity: " << capacity << " bytes" << std::endl;
    
    std::cout << "=========================================" << std::endl;

    // Client creation and message sending
    // Note: client already created above for capacity estimation

    // Ping teszt
    std::cout << "\nDNS server ping test..." << std::endl;
    auto ping_result = client.ping_dns_server();
    if (ping_result) {
        std::cout << "Ping successful: " << ping_result.value().count() << "ms" << std::endl;
    } else {
        std::cout << "Ping failed, but trying to send message..." << std::endl;
    }

    // Phase 3: Enhanced message sending
    std::cout << "\nSending message using Phase 3 steganographic encoding..." << std::endl;
    
    // Convert message to binary data for Phase 3 methods
    std::vector<uint8_t> message_data(message.begin(), message.end());
    
    // Use the new send_data method which supports all Phase 3 features
    auto result = client.send_data(message_data);
    if (result) {
        const auto& send_result = result.value();
        std::cout << "\nMessage sent successfully!" << std::endl;
        std::cout << "Bytes sent: " << send_result.bytes_sent << std::endl;
        std::cout << "Latency: " << send_result.latency.count() << "ms" << std::endl;
        std::cout << "Used domain: " << send_result.used_domain << std::endl;
        
        // Phase 3: Enhanced result information
        std::cout << "--- Phase 3 Results ---" << std::endl;
        std::cout << "Fragments sent: " << send_result.fragments_sent << std::endl;
        std::cout << "Encoding used: " << strategy_name << std::endl;
        std::cout << "Compression used: " << (send_result.compression_used ? "yes" : "no") << std::endl;
        
        std::cout << "Record types used: ";
        for (size_t i = 0; i < send_result.used_record_types.size(); ++i) {
            if (i > 0) std::cout << ", ";
            switch (send_result.used_record_types[i]) {
                case chimera::DnsType::A: std::cout << "A"; break;
                case chimera::DnsType::AAAA: std::cout << "AAAA"; break;
                case chimera::DnsType::TXT: std::cout << "TXT"; break;
                default: std::cout << "Other"; break;
            }
        }
        std::cout << std::endl;
        
        return 0;
    } else {
        std::cout << "\nSend error!" << std::endl;
        return 1;
    }
}
