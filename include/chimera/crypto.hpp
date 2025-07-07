#pragma once

#include <vector>
#include <string>
#include "tl/expected.hpp"
#include <memory>

// Kriptográfiai réteg - AEAD + hibrid kulcscsere
// X25519 + ML-KEM768 kombináció a post-quantum biztonságért
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

// Alapvető típusok
using CryptoKey = std::vector<uint8_t>;
using Nonce = std::vector<uint8_t>;
using Plaintext = std::vector<uint8_t>;
using Ciphertext = std::vector<uint8_t>;
using AssociatedData = std::vector<uint8_t>;
using PublicKey = std::vector<uint8_t>;
using PrivateKey = std::vector<uint8_t>;
using SharedSecret = std::vector<uint8_t>;

// AEAD titkosított csomag
struct EncryptedPacket {
    Ciphertext data;
    Nonce nonce;
};

// Hibrid kulcscsere kulcspár
struct HybridKeyPair {
    PublicKey x25519_public;
    PrivateKey x25519_private;
    PublicKey mlkem_public;
    PrivateKey mlkem_private;
};

// Hibrid kulcscsere eredmény (renamed to avoid conflict)
struct HybridKeyExchangeResult {
    SharedSecret shared_secret;
    Ciphertext mlkem_ciphertext;
};

// AEAD osztály a ChaCha20-Poly1305 kezeléshez
class AEAD {
public:
    // Libsodium inicializálás
    AEAD();

    // Kulcs generálás ChaCha20-Poly1305-höz
    static tl::expected<CryptoKey, CryptoError> generate_key();

    // Titkosítás
    static tl::expected<EncryptedPacket, CryptoError> encrypt(
        const Plaintext& message,
        const CryptoKey& key,
        const AssociatedData& ad = {}
    );

    // Visszafejtés
    static tl::expected<Plaintext, CryptoError> decrypt(
        const EncryptedPacket& packet,
        const CryptoKey& key,
        const AssociatedData& ad = {}
    );
};

// Hibrid kulcscsere osztály - X25519 + ML-KEM768
class HybridKeyExchange {
public:
    // Inicializálás
    HybridKeyExchange();

    // Kulcspár generálás (X25519 + ML-KEM768)
    static tl::expected<HybridKeyPair, CryptoError> generate_keypair();

    // Kliens oldali kulcscsere kezdeményezés
    static tl::expected<HybridKeyExchangeResult, CryptoError> initiate_exchange(
        const PublicKey& server_x25519_public,
        const PublicKey& server_mlkem_public
    );

    // Szerver oldali kulcscsere válasz
    static tl::expected<SharedSecret, CryptoError> respond_to_exchange(
        const HybridKeyPair& server_keypair,
        const PublicKey& client_x25519_public,
        const Ciphertext& client_mlkem_ciphertext
    );

    // Kulcs deriválás a hibrid megosztott titkokból
    static tl::expected<CryptoKey, CryptoError> derive_key(
        const SharedSecret& shared_secret,
        const std::string& info = "CHIMERA v1.0"
    );

private:
    // X25519 kulcscsere segédfüggvények
    static tl::expected<SharedSecret, CryptoError> x25519_exchange(
        const PrivateKey& private_key,
        const PublicKey& public_key
    );

    // ML-KEM768 kulcscsere segédfüggvények
    static tl::expected<std::pair<SharedSecret, Ciphertext>, CryptoError> mlkem_encapsulate(
        const PublicKey& public_key
    );

    static tl::expected<SharedSecret, CryptoError> mlkem_decapsulate(
        const PrivateKey& private_key,
        const Ciphertext& ciphertext
    );

    // HKDF kulcs deriválás
    static tl::expected<CryptoKey, CryptoError> hkdf_expand(
        const SharedSecret& shared_secret,
        const std::string& info,
        size_t key_length
    );
};

} // namespace chimera
