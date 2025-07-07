#include "chimera/crypto.hpp"
#include <sodium.h>
#include <iostream>
#include <cstring>

// Megjegyzés: ML-KEM768 implementáció jelenleg hiányzik
// Ez egy placeholder implementáció a struktúra bemutatásához
// Éles használathoz szükséges egy teljes ML-KEM768 implementáció

namespace chimera {

// AEAD konstruktor - libsodium inicializálás
AEAD::AEAD() {
    if (sodium_init() < 0) {
        std::cerr << "FATAL: libsodium initialization failed!" << std::endl;
        throw std::runtime_error("Sodium initialization failed");
    }
}

tl::expected<CryptoKey, CryptoError> AEAD::generate_key() {
    CryptoKey key(crypto_aead_chacha20poly1305_ietf_KEYBYTES);
    crypto_aead_chacha20poly1305_ietf_keygen(key.data());
    return key;
}

tl::expected<EncryptedPacket, CryptoError> AEAD::encrypt(
    const Plaintext& message,
    const CryptoKey& key,
    const AssociatedData& ad) {

    if (key.size() != crypto_aead_chacha20poly1305_ietf_KEYBYTES) {
        return tl::unexpected(CryptoError::InvalidKeyOrNonce);
    }

    // Nonce generálás minden titkosításnál
    Nonce nonce(crypto_aead_chacha20poly1305_ietf_NPUBBYTES);
    randombytes_buf(nonce.data(), nonce.size());

    Ciphertext ciphertext(message.size() + crypto_aead_chacha20poly1305_ietf_ABYTES);
    unsigned long long ciphertext_len;

    int result = crypto_aead_chacha20poly1305_ietf_encrypt(
        ciphertext.data(), &ciphertext_len,
        message.data(), message.size(),
        ad.data(), ad.size(),
        nullptr,
        nonce.data(), key.data()
    );

    if (result != 0) {
        return tl::unexpected(CryptoError::EncryptionFailed);
    }

    ciphertext.resize(ciphertext_len);
    return EncryptedPacket{ .data = ciphertext, .nonce = nonce };
}

tl::expected<Plaintext, CryptoError> AEAD::decrypt(
    const EncryptedPacket& packet,
    const CryptoKey& key,
    const AssociatedData& ad) {

    if (key.size() != crypto_aead_chacha20poly1305_ietf_KEYBYTES ||
        packet.nonce.size() != crypto_aead_chacha20poly1305_ietf_NPUBBYTES) {
        return tl::unexpected(CryptoError::InvalidKeyOrNonce);
    }

    Plaintext decrypted_message(packet.data.size());
    unsigned long long decrypted_len;

    int result = crypto_aead_chacha20poly1305_ietf_decrypt(
        decrypted_message.data(), &decrypted_len,
        nullptr,
        packet.data.data(), packet.data.size(),
        ad.data(), ad.size(),
        packet.nonce.data(), key.data()
    );

    if (result != 0) {
        return tl::unexpected(CryptoError::DecryptionFailed);
    }

    decrypted_message.resize(decrypted_len);
    return decrypted_message;
}

// Hibrid kulcscsere implementáció
HybridKeyExchange::HybridKeyExchange() {
    // Libsodium már inicializálva van az AEAD konstruktorban
    if (sodium_init() < 0) {
        throw std::runtime_error("Sodium initialization failed");
    }
}

tl::expected<HybridKeyPair, CryptoError> HybridKeyExchange::generate_keypair() {
    HybridKeyPair keypair;

    // X25519 kulcspár generálás
    keypair.x25519_public.resize(crypto_box_PUBLICKEYBYTES);
    keypair.x25519_private.resize(crypto_box_SECRETKEYBYTES);

    if (crypto_box_keypair(keypair.x25519_public.data(), keypair.x25519_private.data()) != 0) {
        return tl::unexpected(CryptoError::KeyGenerationFailed);
    }

    // ML-KEM768 kulcspár generálás (placeholder)
    // Valódi implementációhoz szükséges ML-KEM768 library
    keypair.mlkem_public.resize(1184);  // ML-KEM768 public key méret
    keypair.mlkem_private.resize(2400); // ML-KEM768 private key méret

    // Placeholder random adatok (NEM BIZTONSÁGOS éles használatra!)
    randombytes_buf(keypair.mlkem_public.data(), keypair.mlkem_public.size());
    randombytes_buf(keypair.mlkem_private.data(), keypair.mlkem_private.size());

    return keypair;
}

tl::expected<HybridKeyExchangeResult, CryptoError> HybridKeyExchange::initiate_exchange(
    const PublicKey& server_x25519_public,
    const PublicKey& server_mlkem_public) {

    // Kliens oldali kulcspár generálás
    auto client_keypair_result = generate_keypair();
    if (!client_keypair_result) {
        return tl::unexpected(client_keypair_result.error());
    }

    auto client_keypair = client_keypair_result.value();

    // X25519 kulcscsere
    auto x25519_secret_result = x25519_exchange(client_keypair.x25519_private, server_x25519_public);
    if (!x25519_secret_result) {
        return tl::unexpected(x25519_secret_result.error());
    }

    // ML-KEM768 enkapszuláció
    auto mlkem_result = mlkem_encapsulate(server_mlkem_public);
    if (!mlkem_result) {
        return tl::unexpected(mlkem_result.error());
    }

    // Hibrid megosztott titok kombinálás
    auto x25519_secret = x25519_secret_result.value();
    auto mlkem_secret = mlkem_result.value().first;
    auto mlkem_ciphertext = mlkem_result.value().second;

    SharedSecret combined_secret;
    combined_secret.reserve(x25519_secret.size() + mlkem_secret.size());
    combined_secret.insert(combined_secret.end(), x25519_secret.begin(), x25519_secret.end());
    combined_secret.insert(combined_secret.end(), mlkem_secret.begin(), mlkem_secret.end());

    return HybridKeyExchangeResult{
        .shared_secret = combined_secret,
        .mlkem_ciphertext = mlkem_ciphertext
    };
}

tl::expected<SharedSecret, CryptoError> HybridKeyExchange::respond_to_exchange(
    const HybridKeyPair& server_keypair,
    const PublicKey& client_x25519_public,
    const Ciphertext& client_mlkem_ciphertext) {

    // X25519 kulcscsere
    auto x25519_secret_result = x25519_exchange(server_keypair.x25519_private, client_x25519_public);
    if (!x25519_secret_result) {
        return tl::unexpected(x25519_secret_result.error());
    }

    // ML-KEM768 dekapszuláció
    auto mlkem_secret_result = mlkem_decapsulate(server_keypair.mlkem_private, client_mlkem_ciphertext);
    if (!mlkem_secret_result) {
        return tl::unexpected(mlkem_secret_result.error());
    }

    // Hibrid megosztott titok kombinálás
    auto x25519_secret = x25519_secret_result.value();
    auto mlkem_secret = mlkem_secret_result.value();

    SharedSecret combined_secret;
    combined_secret.reserve(x25519_secret.size() + mlkem_secret.size());
    combined_secret.insert(combined_secret.end(), x25519_secret.begin(), x25519_secret.end());
    combined_secret.insert(combined_secret.end(), mlkem_secret.begin(), mlkem_secret.end());

    return combined_secret;
}

tl::expected<CryptoKey, CryptoError> HybridKeyExchange::derive_key(
    const SharedSecret& shared_secret,
    const std::string& info) {

    return hkdf_expand(shared_secret, info, crypto_aead_chacha20poly1305_ietf_KEYBYTES);
}

// Privát segédfüggvények
tl::expected<SharedSecret, CryptoError> HybridKeyExchange::x25519_exchange(
    const PrivateKey& private_key,
    const PublicKey& public_key) {

    if (private_key.size() != crypto_box_SECRETKEYBYTES ||
        public_key.size() != crypto_box_PUBLICKEYBYTES) {
        return tl::unexpected(CryptoError::InvalidPublicKey);
    }

    SharedSecret shared_secret(crypto_box_BEFORENMBYTES);

    if (crypto_box_beforenm(shared_secret.data(), public_key.data(), private_key.data()) != 0) {
        return tl::unexpected(CryptoError::KeyExchangeFailed);
    }

    return shared_secret;
}

tl::expected<std::pair<SharedSecret, Ciphertext>, CryptoError> HybridKeyExchange::mlkem_encapsulate(
    const PublicKey& public_key) {

    // Placeholder ML-KEM768 implementáció
    // Valódi implementációhoz szükséges ML-KEM768 library
    if (public_key.size() != 1184) {
        return tl::unexpected(CryptoError::InvalidPublicKey);
    }

    SharedSecret shared_secret(32); // ML-KEM768 shared secret méret
    Ciphertext ciphertext(1088);    // ML-KEM768 ciphertext méret

    // Placeholder random adatok (NEM BIZTONSÁGOS!)
    randombytes_buf(shared_secret.data(), shared_secret.size());
    randombytes_buf(ciphertext.data(), ciphertext.size());

    return std::make_pair(shared_secret, ciphertext);
}

tl::expected<SharedSecret, CryptoError> HybridKeyExchange::mlkem_decapsulate(
    const PrivateKey& private_key,
    const Ciphertext& ciphertext) {

    // Placeholder ML-KEM768 implementáció
    if (private_key.size() != 2400 || ciphertext.size() != 1088) {
        return tl::unexpected(CryptoError::InvalidCiphertext);
    }

    SharedSecret shared_secret(32);

    // Placeholder random adatok (NEM BIZTONSÁGOS!)
    randombytes_buf(shared_secret.data(), shared_secret.size());

    return shared_secret;
}

tl::expected<CryptoKey, CryptoError> HybridKeyExchange::hkdf_expand(
    const SharedSecret& shared_secret,
    const std::string& info,
    size_t key_length) {

    // Egyszerű HKDF implementáció libsodium-mal
    CryptoKey derived_key(key_length);

    if (crypto_kdf_blake2b_derive_from_key(
        derived_key.data(), key_length,
        1, info.c_str(),
        shared_secret.data()) != 0) {
        return tl::unexpected(CryptoError::KeyGenerationFailed);
    }

    return derived_key;
}

} // namespace chimera
