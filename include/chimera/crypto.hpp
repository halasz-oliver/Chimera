#pragma once

#include <vector>
#include <string>
#include "tl/expected.hpp"
#include <utility>

// Cryptographic layer - AEAD + hybrid key exchange
// X25519 + ML-KEM768 combination for post-quantum security
namespace chimera {

enum class CryptoError {
    SodiumInitFailed,
    EncryptionFailed,
    DecryptionFailed,
    KeyGenerationFailed,
    InvalidKeyOrNonce,
    KeyExchangeFailed,
    InvalidPublicKey,
    InvalidCiphertext,
    UnsupportedAlgorithm
};

// Basic types
using CryptoKey = std::vector<uint8_t>;
using Nonce = std::vector<uint8_t>;
using Plaintext = std::vector<uint8_t>;
using Ciphertext = std::vector<uint8_t>;
using AssociatedData = std::vector<uint8_t>;
using PublicKey = std::vector<uint8_t>;
using PrivateKey = std::vector<uint8_t>;
using SharedSecret = std::vector<uint8_t>;

// AEAD encrypted package
struct EncryptedPacket {
    Ciphertext data;
    Nonce nonce;
};

// Hybrid key exchange keypair
struct HybridKeyPair {
    PublicKey x25519_public;
    PrivateKey x25519_private;
    PublicKey mlkem_public;
    PrivateKey mlkem_private;
};

// Hybrid key exchange result
struct HybridKeyExchangeResult {
    SharedSecret shared_secret;
    Ciphertext mlkem_ciphertext;
};

// AEAD class for ChaCha20-Poly1305 handling
class AEAD {
public:
    // Libsodium initialization
    AEAD();

    // Key generation for ChaCha20-Poly1305
    static tl::expected<CryptoKey, CryptoError> generate_key();

    // Encryption
    static tl::expected<EncryptedPacket, CryptoError> encrypt(
        const Plaintext& message,
        const CryptoKey& key,
        const AssociatedData& ad = {}
    );

    // Decryption
    static tl::expected<Plaintext, CryptoError> decrypt(
        const EncryptedPacket& packet,
        const CryptoKey& key,
        const AssociatedData& ad = {}
    );
};

// Hybrid key exchange class - X25519 + ML-KEM768
class HybridKeyExchange {
public:
    // Initialization
    HybridKeyExchange();

    // Keypair generation (X25519 + ML-KEM768)
    static tl::expected<HybridKeyPair, CryptoError> generate_keypair();

    // Client-side key exchange initiation
    static tl::expected<HybridKeyExchangeResult, CryptoError> initiate_exchange(
        const PublicKey& server_x25519_public,
        const PublicKey& server_mlkem_public
    );

    // Server-side key exchange response
    static tl::expected<SharedSecret, CryptoError> respond_to_exchange(
        const HybridKeyPair& server_keypair,
        const PublicKey& client_x25519_public,
        const Ciphertext& client_mlkem_ciphertext
    );

    // Key derivation from hybrid shared secrets
    static tl::expected<CryptoKey, CryptoError> derive_key(
        const SharedSecret& shared_secret,
        const std::string& info = "CHIMERA v1.0"
    );

private:
    // X25519 key exchange helper functions
    static tl::expected<SharedSecret, CryptoError> x25519_exchange(
        const PrivateKey& private_key,
        const PublicKey& public_key
    );

    // ML-KEM768 key exchange helper functions - PRODUCTION
    static tl::expected<std::pair<SharedSecret, Ciphertext>, CryptoError> mlkem_encapsulate(
        const PublicKey& public_key
    );

    static tl::expected<SharedSecret, CryptoError> mlkem_decapsulate(
        const PrivateKey& private_key,
        const Ciphertext& ciphertext
    );

    // HKDF key derivation
    static tl::expected<CryptoKey, CryptoError> hkdf_expand(
        const SharedSecret& shared_secret,
        const std::string& info,
        size_t key_length
    );
};

} // namespace chimera
