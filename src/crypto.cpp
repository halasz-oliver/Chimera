#include "chimera/crypto.hpp"
#include <sodium.h>
#include <oqs/oqs.h>
#include <iostream>
#include <cstring>

// Production ML-KEM768 implementation with liboqs
namespace chimera {

// AEAD constructor - libsodium initialization
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

    // Nonce generation for every encryption
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

// Hybrid key exchange implementation - PRODUCTION ML-KEM768
HybridKeyExchange::HybridKeyExchange() {
    if (sodium_init() < 0) {
        throw std::runtime_error("Sodium initialization failed");
    }

    // liboqs initialization check
    if (!OQS_KEM_alg_is_enabled(OQS_KEM_alg_kyber_768)) {
        throw std::runtime_error("ML-KEM768 not available in liboqs");
    }
}

tl::expected<HybridKeyPair, CryptoError> HybridKeyExchange::generate_keypair() {
    HybridKeyPair keypair;

    // X25519 keypair generation
    keypair.x25519_public.resize(crypto_box_PUBLICKEYBYTES);
    keypair.x25519_private.resize(crypto_box_SECRETKEYBYTES);

    if (crypto_box_keypair(keypair.x25519_public.data(), keypair.x25519_private.data()) != 0) {
        return tl::unexpected(CryptoError::KeyGenerationFailed);
    }

    // ML-KEM768 keypair generation with liboqs
    OQS_KEM* kem = OQS_KEM_new(OQS_KEM_alg_kyber_768);
    if (!kem) {
        return tl::unexpected(CryptoError::KeyGenerationFailed);
    }

    keypair.mlkem_public.resize(kem->length_public_key);
    keypair.mlkem_private.resize(kem->length_secret_key);

    OQS_STATUS status = OQS_KEM_keypair(kem,
        keypair.mlkem_public.data(),
        keypair.mlkem_private.data());

    OQS_KEM_free(kem);

    if (status != OQS_SUCCESS) {
        return tl::unexpected(CryptoError::KeyGenerationFailed);
    }

    return keypair;
}

tl::expected<HybridKeyExchangeResult, CryptoError> HybridKeyExchange::initiate_exchange(
    const PublicKey& server_x25519_public,
    const PublicKey& server_mlkem_public) {

    // Client-side keypair generation
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

    // ML-KEM768 encapsulation - PRODUCTION
    auto mlkem_result = mlkem_encapsulate(server_mlkem_public);
    if (!mlkem_result) {
        return tl::unexpected(mlkem_result.error());
    }

    // Hybrid shared secret combination
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

    // ML-KEM768 decapsulation - PRODUCTION
    auto mlkem_secret_result = mlkem_decapsulate(server_keypair.mlkem_private, client_mlkem_ciphertext);
    if (!mlkem_secret_result) {
        return tl::unexpected(mlkem_secret_result.error());
    }

    // Hybrid shared secret combination
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

// Private helper functions
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

// PRODUCTION ML-KEM768 encapsulation
tl::expected<std::pair<SharedSecret, Ciphertext>, CryptoError> HybridKeyExchange::mlkem_encapsulate(
    const PublicKey& public_key) {

    OQS_KEM* kem = OQS_KEM_new(OQS_KEM_alg_kyber_768);
    if (!kem) {
        return tl::unexpected(CryptoError::UnsupportedAlgorithm);
    }

    if (public_key.size() != kem->length_public_key) {
        OQS_KEM_free(kem);
        return tl::unexpected(CryptoError::InvalidPublicKey);
    }

    SharedSecret shared_secret(kem->length_shared_secret);
    Ciphertext ciphertext(kem->length_ciphertext);

    OQS_STATUS status = OQS_KEM_encaps(kem,
        ciphertext.data(),
        shared_secret.data(),
        public_key.data());

    OQS_KEM_free(kem);

    if (status != OQS_SUCCESS) {
        return tl::unexpected(CryptoError::KeyExchangeFailed);
    }

    return std::make_pair(shared_secret, ciphertext);
}

// PRODUCTION ML-KEM768 decapsulation
tl::expected<SharedSecret, CryptoError> HybridKeyExchange::mlkem_decapsulate(
    const PrivateKey& private_key,
    const Ciphertext& ciphertext) {

    OQS_KEM* kem = OQS_KEM_new(OQS_KEM_alg_kyber_768);
    if (!kem) {
        return tl::unexpected(CryptoError::UnsupportedAlgorithm);
    }

    if (private_key.size() != kem->length_secret_key ||
        ciphertext.size() != kem->length_ciphertext) {
        OQS_KEM_free(kem);
        return tl::unexpected(CryptoError::InvalidCiphertext);
    }

    SharedSecret shared_secret(kem->length_shared_secret);

    OQS_STATUS status = OQS_KEM_decaps(kem,
        shared_secret.data(),
        ciphertext.data(),
        private_key.data());

    OQS_KEM_free(kem);

    if (status != OQS_SUCCESS) {
        return tl::unexpected(CryptoError::KeyExchangeFailed);
    }

    return shared_secret;
}

tl::expected<CryptoKey, CryptoError> HybridKeyExchange::hkdf_expand(
    const SharedSecret& shared_secret,
    const std::string& info,
    size_t key_length) {

    // HKDF implementation with libsodium
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
