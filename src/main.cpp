#include "chimera/client.hpp"
#include <iostream>
#include <fstream>
#include <string>

// TODO: proper CLI parsing library (argparse vagy hasonló)
// TODO: config file support (JSON/YAML)

void print_usage(const char* program_name) {
    std::cout << "Használat: " << program_name << " [opciók] [üzenet_fájl]\n";
    std::cout << "Opciók:\n";
    std::cout << "  -s, --server <ip>     DNS server címe (default: 8.8.8.8)\n";
    std::cout << "  -p, --port <port>     DNS server portja (default: 53)\n";
    std::cout << "  -d, --domain <domain> Target domain (default: example.com)\n";
    std::cout << "  -t, --timeout <ms>    Timeout milliszekundumban (default: 5000)\n";
    std::cout << "  -h, --help           Ez a súgó\n";
    std::cout << "\nPéldák:\n";
    std::cout << "  " << program_name << " message.txt\n";
    std::cout << "  " << program_name << " -s 1.1.1.1 -d test.com message.txt\n";
}

int main(int argc, char** argv) {
    chimera::ClientConfig config;
    std::string message = "Hello from Chimera! 🦎"; // emoji mert miért ne

    // Nagyon primitív argument parsing
    // TODO: ezt ki kell váltani egy rendes library-vel
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            print_usage(argv[0]);
            return 0;
        } else if (arg == "-s" || arg == "--server") {
            if (i + 1 < argc) {
                config.dns_server = argv[++i];
            } else {
                std::cerr << "Hiányzó DNS server cím\n";
                return 1;
            }
        } else if (arg == "-p" || arg == "--port") {
            if (i + 1 < argc) {
                config.dns_port = static_cast<uint16_t>(std::stoi(argv[++i]));
            } else {
                std::cerr << "Hiányzó port szám\n";
                return 1;
            }
        } else if (arg == "-d" || arg == "--domain") {
            if (i + 1 < argc) {
                config.target_domain = argv[++i];
            } else {
                std::cerr << "Hiányzó target domain\n";
                return 1;
            }
        } else if (arg == "-t" || arg == "--timeout") {
            if (i + 1 < argc) {
                config.timeout = std::chrono::milliseconds(std::stoi(argv[++i]));
            } else {
                std::cerr << "Hiányzó timeout érték\n";
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
                std::cout << "Üzenet betöltve fájlból: " << arg << std::endl;
            } else {
                std::cerr << "Nem sikerült megnyitni a fájlt: " << arg << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Ismeretlen opció: " << arg << std::endl;
            print_usage(argv[0]);
            return 1;
        }
    }

    std::cout << "=== CHIMERA DNS Steganography Client ===" << std::endl;
    std::cout << "DNS Server: " << config.dns_server << ":" << config.dns_port << std::endl;
    std::cout << "Target Domain: " << config.target_domain << std::endl;
    std::cout << "Timeout: " << config.timeout.count() << "ms" << std::endl;
    std::cout << "Üzenet hossza: " << message.size() << " karakter" << std::endl;
    std::cout << "=========================================" << std::endl;

    // Client létrehozás és üzenet küldés
    chimera::ChimeraClient client(config);

    // Ping teszt
    std::cout << "\nDNS server ping teszt..." << std::endl;
    auto ping_result = client.ping_dns_server();
    if (ping_result) {
        std::cout << "Ping sikeres: " << ping_result.value().count() << "ms" << std::endl;
    } else {
        std::cout << "Ping sikertelen, de próbáljuk az üzenet küldést..." << std::endl;
    }

    // Üzenet küldés
    std::cout << "\nÜzenet küldése..." << std::endl;
    auto result = client.send_text(message);

    if (result) {
        const auto& send_result = result.value();
        std::cout << "\nSikeres küldés!" << std::endl;
        std::cout << "Elküldött bájtok: " << send_result.bytes_sent << std::endl;
        std::cout << "Latencia: " << send_result.latency.count() << "ms" << std::endl;
        std::cout << "Használt domain: " << send_result.used_domain << std::endl;
        return 0;
    } else {
        std::cout << "\nKüldési hiba!" << std::endl;
        // TODO: részletes hibakód kiírás
        return 1;
    }
}
