#include "chimera/client.hpp"
#include <iostream>
#include <fstream>
#include <string>

// Egyszerű CLI parsing - később cseréljük le egy rendes library-re
void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options] [message_file]\n";
    std::cout << "Options:\n";
    std::cout << "  -s, --server    DNS server address (default: 8.8.8.8)\n";
    std::cout << "  -p, --port      DNS server port (default: 53)\n";
    std::cout << "  -d, --domain    Target domain (default: example.com)\n";
    std::cout << "  -t, --timeout   Timeout in milliseconds (default: 5000)\n";
    std::cout << "  -h, --help      Show this help\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << program_name << " message.txt\n";
    std::cout << "  " << program_name << " -s 1.1.1.1 -d test.com message.txt\n";
}

int main(int argc, char** argv) {
    chimera::ClientConfig config;
    std::string message = "Hello from Chimera! 🦎";

    // Alapvető argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            print_usage(argv[0]);
            return 0;
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
        } else if (arg[0] != '-') {
            // Fájlnév
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

    std::cout << "=== CHIMERA DNS Steganography Client ===" << std::endl;
    std::cout << "DNS Server: " << config.dns_server << ":" << config.dns_port << std::endl;
    std::cout << "Target Domain: " << config.target_domain << std::endl;
    std::cout << "Timeout: " << config.timeout.count() << "ms" << std::endl;
    std::cout << "Message length: " << message.size() << " characters" << std::endl;
    std::cout << "=========================================" << std::endl;

    // Kliens létrehozás és üzenet küldés
    chimera::ChimeraClient client(config);

    // Ping teszt
    std::cout << "\nDNS server ping test..." << std::endl;
    auto ping_result = client.ping_dns_server();
    if (ping_result) {
        std::cout << "Ping successful: " << ping_result.value().count() << "ms" << std::endl;
    } else {
        std::cout << "Ping failed, but trying to send message..." << std::endl;
    }

    // Üzenet küldés
    std::cout << "\nSending message..." << std::endl;
    auto result = client.send_text(message);
    if (result) {
        const auto& send_result = result.value();
        std::cout << "\nMessage sent successfully!" << std::endl;
        std::cout << "Bytes sent: " << send_result.bytes_sent << std::endl;
        std::cout << "Latency: " << send_result.latency.count() << "ms" << std::endl;
        std::cout << "Used domain: " << send_result.used_domain << std::endl;
        return 0;
    } else {
        std::cout << "\nSend error!" << std::endl;
        return 1;
    }
}
