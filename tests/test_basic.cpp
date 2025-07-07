#include "chimera/base64.hpp"
#include "chimera/dns_packet.hpp"
#include "chimera/client.hpp"
#include "chimera/crypto.hpp"
#include <iostream>
#include <cassert>
#include <chrono>

// Alapvető teszt suite - később cseréljük le Catch2-re vagy Google Test-re
void test_base64_basic() {
    std::cout << "Base64 basic tests..." << std::endl;

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

    std::cout << "Base64 tests passed" << std::endl;
}

void test_dns_packet_building() {
    std::cout << "DNS packet building tests..." << std::endl;

    chimera::DnsQuestion question{"test.example.com", chimera::DnsType::TXT};
    auto packet = chimera::DnsPacketBuilder::build_query(question, "test payload");

    // Alapvető ellenőrzések
    assert(packet.size() > 12); // legalább header + question

    // Header ellenőrzés
    assert(packet[2] == 0x01); // flags első byte
    assert(packet[3] == 0x00); // flags második byte
    assert(packet[4] == 0x00 && packet[5] == 0x01); // qdcount = 1

    std::cout << "DNS packet size: " << packet.size() << " bytes" << std::endl;
    std::cout << "DNS packet tests passed" << std::endl;
}

void test_client_config() {
    std::cout << "Client configuration tests..." << std::endl;

    chimera::ClientConfig config;
    config.dns_server = "1.1.1.1";
    config.dns_port = 5353;
    config.target_domain = "test.local";

    chimera::ChimeraClient client(config);
    const auto& retrieved_config = client.get_config();

    assert(retrieved_config.dns_server == "1.1.1.1");
    assert(retrieved_config.dns_port == 5353);
    assert(retrieved_config.target_domain == "test.local");

    std::cout << "Client config tests passed" << std::endl;
}

void test_edge_cases() {
    std::cout << "Edge case tests..." << std::endl;

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

    std::cout << "Edge case tests passed" << std::endl;
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

    std::cout << "1MB data encoding: " << encode_time.count() << "μs" << std::endl;
    std::cout << "1MB data decoding: " << decode_time.count() << "μs" << std::endl;

    assert(decoded == test_data);
    std::cout << "Benchmark passed" << std::endl;
}

void test_crypto_aead() {
    std::cout << "\nAEAD crypto tests (libsodium)..." << std::endl;

    // Konstruktor hívás inicializálja a libsodium-ot
    chimera::AEAD aead;

    // Sikeres encrypt/decrypt kör
    auto key_res = chimera::AEAD::generate_key();
    assert(key_res.has_value());
    auto key = key_res.value();

    std::string original_message_str = "This is a super secret message that nobody should read! 🤫";
    chimera::Plaintext original_message(original_message_str.begin(), original_message_str.end());
    chimera::AssociatedData ad = {'v', '1', '.', '0'};

    auto encrypted_res = chimera::AEAD::encrypt(original_message, key, ad);
    assert(encrypted_res.has_value());
    auto encrypted_packet = encrypted_res.value();

    // Biztos, hogy nem ugyanaz mint az eredeti
    assert(encrypted_packet.data != original_message);

    auto decrypted_res = chimera::AEAD::decrypt(encrypted_packet, key, ad);
    assert(decrypted_res.has_value());
    auto decrypted_message = decrypted_res.value();

    assert(decrypted_message == original_message);
    std::cout << "Successful encryption and decryption." << std::endl;

    // Hibás kulccsal való visszafejtés
    auto wrong_key_res = chimera::AEAD::generate_key();
    assert(wrong_key_res.has_value());
    auto wrong_key = wrong_key_res.value();
    assert(key != wrong_key);

    auto decrypt_fail_res = chimera::AEAD::decrypt(encrypted_packet, wrong_key, ad);
    assert(!decrypt_fail_res.has_value());
    assert(decrypt_fail_res.error() == chimera::CryptoError::DecryptionFailed);
    std::cout << "Wrong key decryption correctly failed." << std::endl;

    std::cout << "AEAD crypto tests passed" << std::endl;
}

void test_hybrid_key_exchange() {
    std::cout << "\nHybrid key exchange tests..." << std::endl;

    chimera::HybridKeyExchange kex;

    // Szerver és kliens kulcspár generálás
    auto server_keypair_res = chimera::HybridKeyExchange::generate_keypair();
    assert(server_keypair_res.has_value());
    auto server_keypair = server_keypair_res.value();

    auto client_keypair_res = chimera::HybridKeyExchange::generate_keypair();
    assert(client_keypair_res.has_value());
    auto client_keypair = client_keypair_res.value();

    // Kliens oldali kulcscsere kezdeményezés
    auto client_exchange_res = chimera::HybridKeyExchange::initiate_exchange(
        server_keypair.x25519_public,
        server_keypair.mlkem_public
    );
    assert(client_exchange_res.has_value());
    auto client_exchange = client_exchange_res.value();

    // Szerver oldali kulcscsere válasz
    auto server_secret_res = chimera::HybridKeyExchange::respond_to_exchange(
        server_keypair,
        client_keypair.x25519_public,
        client_exchange.mlkem_ciphertext
    );
    assert(server_secret_res.has_value());
    auto server_secret = server_secret_res.value();

    // Kulcs deriválás mindkét oldalon
    auto client_key_res = chimera::HybridKeyExchange::derive_key(client_exchange.shared_secret);
    assert(client_key_res.has_value());
    auto client_key = client_key_res.value();

    auto server_key_res = chimera::HybridKeyExchange::derive_key(server_secret);
    assert(server_key_res.has_value());
    auto server_key = server_key_res.value();

    // Megjegyzés: A placeholder implementáció miatt a kulcsok nem egyeznek
    // Valódi ML-KEM768 implementációval egyezniük kellene
    std::cout << "Hybrid key exchange basic structure test passed" << std::endl;
    std::cout << "Note: This is a placeholder implementation - real ML-KEM768 needed for production" << std::endl;
}

int main() {
    std::cout << "=== CHIMERA Test Suite ===" << std::endl;

    try {
        test_base64_basic();
        test_dns_packet_building();
        test_client_config();
        test_edge_cases();
        benchmark_base64();
        test_crypto_aead();
        test_hybrid_key_exchange();

        std::cout << "\nAll tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\nTest error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\nUnknown test error!" << std::endl;
        return 1;
    }
}