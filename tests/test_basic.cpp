#include "chimera/base64.hpp"
#include "chimera/dns_packet.hpp"
#include "chimera/client.hpp"
#include <iostream>
#include <cassert>
#include <vector>

// TODO: proper test framework (Catch2 vagy Google Test)
// TODO: benchmark tesztek
// TODO: fuzz testing

void test_base64_basic() {
    std::cout << "Base64 alap tesztek..." << std::endl;

    // Alap teszt
    auto encoded = chimera::Base64::encode("Hello World!");
    auto decoded = chimera::Base64::decode(encoded);
    assert(decoded == "Hello World!");

    // Üres string teszt
    assert(chimera::Base64::encode("").empty());
    assert(chimera::Base64::decode("").empty());

    // Padding tesztek
    assert(chimera::Base64::encode("A") == "QQ==");
    assert(chimera::Base64::encode("AB") == "QUI=");
    assert(chimera::Base64::encode("ABC") == "QUJD");

    // Visszafejtés tesztek
    assert(chimera::Base64::decode("QQ==") == "A");
    assert(chimera::Base64::decode("QUI=") == "AB");
    assert(chimera::Base64::decode("QUJD") == "ABC");

    std::cout << "Base64 tesztek sikeresek" << std::endl;
}

void test_dns_packet_building() {
    std::cout << "DNS packet building tesztek..." << std::endl;

    chimera::DnsQuestion question{"test.example.com", chimera::DnsType::TXT};
    auto packet = chimera::DnsPacketBuilder::build_query(question, "test payload");

    // Alapvető ellenőrzések
    assert(packet.size() > 12); // legalább header + question

    // Header ellenőrzés (első 12 byte)
    assert(packet[2] == 0x01); // flags első byte
    assert(packet[3] == 0x00); // flags második byte
    assert(packet[4] == 0x00 && packet[5] == 0x01); // qdcount = 1

    std::cout << "DNS packet méret: " << packet.size() << " byte" << std::endl;
    std::cout << "DNS packet tesztek sikeresek" << std::endl;
}

void test_client_config() {
    std::cout << "Client konfiguráció tesztek..." << std::endl;

    chimera::ClientConfig config;
    config.dns_server = "1.1.1.1";
    config.dns_port = 5353;
    config.target_domain = "test.local";

    chimera::ChimeraClient client(config);

    const auto& retrieved_config = client.get_config();
    assert(retrieved_config.dns_server == "1.1.1.1");
    assert(retrieved_config.dns_port == 5353);
    assert(retrieved_config.target_domain == "test.local");

    std::cout << "Client config tesztek sikeresek" << std::endl;
}

void test_edge_cases() {
    std::cout << "Edge case tesztek..." << std::endl;

    // Nagy string base64 kódolás
    std::string big_string(1000, 'A');
    auto encoded_big = chimera::Base64::encode(big_string);
    auto decoded_big = chimera::Base64::decode(encoded_big);
    assert(decoded_big == big_string);

    // Unicode karakterek (UTF-8)
    std::string unicode_test = "Hello 🌍 World! áéíóú";
    auto encoded_unicode = chimera::Base64::encode(unicode_test);
    auto decoded_unicode = chimera::Base64::decode(encoded_unicode);
    assert(decoded_unicode == unicode_test);

    std::cout << "Edge case tesztek sikeresek" << std::endl;
}

void benchmark_base64() {
    std::cout << "Base64 benchmark..." << std::endl;

    // 1MB adat generálás
    std::string test_data(1024 * 1024, 'X');

    auto start = std::chrono::high_resolution_clock::now();
    auto encoded = chimera::Base64::encode(test_data);
    auto encode_end = std::chrono::high_resolution_clock::now();
    auto decoded = chimera::Base64::decode(encoded);
    auto decode_end = std::chrono::high_resolution_clock::now();

    auto encode_time = std::chrono::duration_cast<std::chrono::microseconds>(encode_end - start);
    auto decode_time = std::chrono::duration_cast<std::chrono::microseconds>(decode_end - encode_end);

    std::cout << "1MB adat kódolás: " << encode_time.count() << "μs" << std::endl;
    std::cout << "1MB adat dekódolás: " << decode_time.count() << "μs" << std::endl;

    assert(decoded == test_data);
    std::cout << "Benchmark sikeres" << std::endl;
}

int main() {
    std::cout << "=== CHIMERA Test Suite ===" << std::endl;

    try {
        test_base64_basic();
        test_dns_packet_building();
        test_client_config();
        test_edge_cases();
        benchmark_base64();

        std::cout << "\nMinden teszt sikeres!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\nTeszt hiba: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\nIsmeretlen teszt hiba!" << std::endl;
        return 1;
    }
}
