#include "chimera/crypto.hpp"
#include <iostream>
#include <stdexcept>

namespace chimera {

// Konstruktor, ami gondoskodik a libsodium inicializálásáról.
// Singleton-szerűen, csak egyszer fut le. Ha ez -1, akkor game over.
AEAD::AEAD() {
    if (sodium_init() < 0) {
        // Pánik! Nem indult el a libsodium. Ezt nem tudjuk kezelni.
        std::cerr << "FATAL: libsodium could not be initialized!" << std::endl;
        throw std::runtime_error("Sodium initialization failed");
    }
}

tl::expected<CryptoKey, CryptoError> AEAD::generate_key() {
    CryptoKey key(crypto_aead_chacha20poly1305_ietf_KEYBYTES);
    crypto_aead_chacha20poly1305_ietf_keygen(key.data());
    return key;
}

tl::expected<EncryptedPacket, CryptoError> AEAD::encrypt(const Plaintext& message, const CryptoKey& key, const AssociatedData& ad) {
    if (key.size() != crypto_aead_chacha20poly1305_ietf_KEYBYTES) {
        return tl::unexpected(CryptoError::InvalidKeyOrNonce);
    }

    // A nonce-t minden üzenetnél újra kell generálni, különben sebezhetővé válunk!
    Nonce nonce(crypto_aead_chacha20poly1305_ietf_NPUBBYTES);
    randombytes_buf(nonce.data(), nonce.size());

    Ciphertext ciphertext(message.size() + crypto_aead_chacha20poly1305_ietf_ABYTES);
    unsigned long long ciphertext_len;

    int result = crypto_aead_chacha20poly1305_ietf_encrypt(
        ciphertext.data(), &ciphertext_len,
        message.data(), message.size(),
        ad.data(), ad.size(),
        nullptr, // nsec, nem használjuk
        nonce.data(), key.data());

    if (result != 0) {
        // Valamiért nem sikerült a titkosítás. Lehet rossz a kulcs? Vagy a memória?
        return tl::unexpected(CryptoError::EncryptionFailed);
    }
    ciphertext.resize(ciphertext_len); // A valós méretre vágjuk

    return EncryptedPacket{ .data = ciphertext, .nonce = nonce };
}


tl::expected<Plaintext, CryptoError> AEAD::decrypt(const EncryptedPacket& packet, const CryptoKey& key, const AssociatedData& ad) {
    if (key.size() != crypto_aead_chacha20poly1305_ietf_KEYBYTES || packet.nonce.size() != crypto_aead_chacha20poly1305_ietf_NPUBBYTES) {
        return tl::unexpected(CryptoError::InvalidKeyOrNonce);
    }

    Plaintext decrypted_message(packet.data.size());
    unsigned long long decrypted_len;

    int result = crypto_aead_chacha20poly1305_ietf_decrypt(
        decrypted_message.data(), &decrypted_len,
        nullptr, // nsec, nem használjuk
        packet.data.data(), packet.data.size(),
        ad.data(), ad.size(),
        packet.nonce.data(), key.data());

    if (result != 0) {
        // SIKERTELEN VISSZAFEJTÉS! Ez lehet támadás (megváltoztatták az üzenetet),
        // vagy csak simán rossz kulcsot adtunk meg. Mindenesetre az üzenet kuka.
        return tl::unexpected(CryptoError::DecryptionFailed);
    }
    decrypted_message.resize(decrypted_len); // Méretre vágás

    return decrypted_message;
}

} // namespace chimera
