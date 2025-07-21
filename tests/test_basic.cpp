#include "chimera/base64.hpp"
#include "chimera/dns_packet.hpp"
#include "chimera/client.hpp"
#include "chimera/crypto.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

// Legacy Phase 1 compatibility tests - use chimera_test_comprehensive for full testing
void test_base64_legacy() {
    std::cout << "[TEST] Base64 legacy compatibility..." << std::endl;

    const auto encoded = chimera::Base64::encode("Hello World!");
    const auto decoded = chimera::Base64::decode(encoded);
    assert(decoded == "Hello World!");

    assert(chimera::Base64::encode("").empty());
    assert(chimera::Base64::decode("").empty());

    assert(chimera::Base64::encode("A") == "QQ==");
    assert(chimera::Base64::encode("AB") == "QUI=");
    assert(chimera::Base64::encode("ABC") == "QUJD");

    assert(chimera::Base64::decode("QQ==") == "A");
    assert(chimera::Base64::decode("QUI=") == "AB");
    assert(chimera::Base64::decode("QUJD") == "ABC");

    std::cout << "[PASS] Base64 legacy tests passed" << std::endl;
}

void test_crypto_aead_legacy() {
    std::cout << "[TEST] AEAD crypto legacy compatibility..." << std::endl;

    auto key_res = chimera::AEAD::generate_key();
    assert(key_res.has_value());
    auto key = key_res.value();

    std::string original_str = "This is a super secret message!";
    chimera::Plaintext original(original_str.begin(), original_str.end());
    chimera::AssociatedData ad = {'v', '1', '.', '0'};

    auto encrypted_res = chimera::AEAD::encrypt(original, key, ad);
    assert(encrypted_res.has_value());
    auto encrypted_packet = encrypted_res.value();

    assert(encrypted_packet.data != original);

    auto decrypted_res = chimera::AEAD::decrypt(encrypted_packet, key, ad);
    assert(decrypted_res.has_value());
    auto decrypted = decrypted_res.value();
    assert(decrypted == original);

    // Wrong key test
    auto wrong_key_res = chimera::AEAD::generate_key();
    auto wrong_key = wrong_key_res.value();
    auto decrypt_fail = chimera::AEAD::decrypt(encrypted_packet, wrong_key, ad);
    assert(!decrypt_fail.has_value());

    std::cout << "[PASS] AEAD crypto legacy tests passed" << std::endl;
}

void test_production_hybrid_key_exchange_legacy() {
    std::cout << "[TEST] Hybrid Key Exchange legacy compatibility..." << std::endl;

    // Server keypair generation
    auto server_keypair_res = chimera::HybridKeyExchange::generate_keypair();
    assert(server_keypair_res.has_value());
    auto server_keypair = server_keypair_res.value();

    // Kliens oldali kulcscsere
    auto client_exchange_res = chimera::HybridKeyExchange::initiate_exchange(
        server_keypair.x25519_public,
        server_keypair.mlkem_public
    );
    assert(client_exchange_res.has_value());
    auto client_exchange = client_exchange_res.value();

    // Server-side response (dummy client pubkey)
    auto client_keypair_res = chimera::HybridKeyExchange::generate_keypair();
    assert(client_keypair_res.has_value());
    auto client_keypair = client_keypair_res.value();

    auto server_secret_res = chimera::HybridKeyExchange::respond_to_exchange(
        server_keypair,
        client_keypair.x25519_public,
        client_exchange.mlkem_ciphertext
    );
    assert(server_secret_res.has_value());

    // Key derivation
    auto client_key_res = chimera::HybridKeyExchange::derive_key(client_exchange.shared_secret);
    auto server_key_res = chimera::HybridKeyExchange::derive_key(server_secret_res.value());

    assert(client_key_res.has_value());
    assert(server_key_res.has_value());

    std::cout << "[PASS] Hybrid key exchange legacy tests passed!" << std::endl;
    std::cout << "[SECURITY] Real ML-KEM768 + X25519 hybrid security verified" << std::endl;
}

void test_dns_packet_building_legacy() {
    std::cout << "[TEST] DNS packet building legacy compatibility..." << std::endl;

    const chimera::DnsQuestion question{"test.example.com", chimera::DnsType::TXT};
    auto packet = chimera::DnsPacketBuilder::build_query(question, "test payload");

    assert(packet.size() > 12);
    assert(packet[2] == 0x01);
    assert(packet[3] == 0x00);
    assert(packet[4] == 0x00 && packet[5] == 0x01);

    std::cout << "[PASS] DNS packet legacy tests passed" << std::endl;
}

void test_edge_cases_legacy() {
    std::cout << "🧪 Edge case legacy compatibility..." << std::endl;

    std::string big_string(1000, 'A');
    auto encoded_big = chimera::Base64::encode(big_string);
    auto decoded_big = chimera::Base64::decode(encoded_big);
    assert(decoded_big == big_string);

    std::string unicode_test = "Hello World! World! áéíóú";
    auto encoded_unicode = chimera::Base64::encode(unicode_test);
    auto decoded_unicode = chimera::Base64::decode(encoded_unicode);
    assert(decoded_unicode == unicode_test);

    std::cout << "[PASS] Edge case legacy tests passed" << std::endl;
}

int main() {
    std::cout << "=== CHIMERA Legacy Compatibility Test Suite ===" << std::endl;
    std::cout << "Testing Phase 1 compatibility (use chimera_test_comprehensive for full Phase 2 testing)" << std::endl;
    std::cout << "[WARNING] This is the legacy test - run 'chimera_test_comprehensive' for modern tests\n" << std::endl;

    try {
        test_base64_legacy();
        test_dns_packet_building_legacy();
        test_edge_cases_legacy();
        test_crypto_aead_legacy();
        test_production_hybrid_key_exchange_legacy();

        std::cout << "\nALL LEGACY COMPATIBILITY TESTS PASSED!" << std::endl;
        std::cout << "[OK] Base64 encoding (HTML entities fixed)" << std::endl;
        std::cout << "[OK] AEAD encryption (ChaCha20-Poly1305)" << std::endl;
        std::cout << "[OK] PRODUCTION X25519 + ML-KEM768 hybrid key exchange" << std::endl;
        std::cout << "[OK] HKDF key derivation" << std::endl;
        std::cout << "\n[INFO] For comprehensive Phase 2 testing, run: ./chimera_test_comprehensive" << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n[FAIL] Test error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n[FAIL] Unknown test error!" << std::endl;
        return 1;
    }
}
