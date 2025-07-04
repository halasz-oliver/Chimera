#pragma once

#include <vector>
#include <string>
#include "tl/expected.hpp"
#include <sodium.h>

// Kriptográfiai alapréteg
// TODO: Post-quantum cuccok integrálása

namespace chimera {

    enum class CryptoError {
        SodiumInitFailed,
        EncryptionFailed,
        DecryptionFailed,
        KeyGenerationFailed,
        InvalidKeyOrNonce
    };

    using CryptoKey = std::vector<unsigned char>;
    using Nonce = std::vector<unsigned char>;
    using Plaintext = std::vector<unsigned char>;
    using Ciphertext = std::vector<unsigned char>;
    using AssociatedData = std::vector<unsigned char>;

    struct EncryptedPacket {
        Ciphertext data;
        Nonce nonce;
    };

    class AEAD {
    public:
        // Biztosítjuk, hogy a libsodium inicializálva legyen.
        // Ha ez hibát dob, akkor nagy a baj.
        AEAD();

        // Generál egy új kulcsot a ChaCha20-Poly1305-höz
        static tl::expected<CryptoKey, CryptoError> generate_key();

        // Titkosítunk mint a nagyok
        static tl::expected<EncryptedPacket, CryptoError> encrypt(const Plaintext& message, const CryptoKey& key, const AssociatedData& ad = {});

        // Visszafejtjük a titkot
        static tl::expected<Plaintext, CryptoError> decrypt(const EncryptedPacket& packet, const CryptoKey& key, const AssociatedData& ad = {});
    };

} // namespace chimera
