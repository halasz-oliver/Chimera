# Security

## Scope
This reflects current code: C++20, libsodium + liboqs, transports
UDP/DoH/DoT, steganography options.

## Crypto
- Use hybrid (use_hybrid_crypto = true) for post-quantum + classical
- AEAD handled by libsodium (per implementation); avoid logging secrets

## Transports
- UDP: fastest, visible; DoH/DoT: encrypted; use DoH for stealth

## Config hardening
```cpp
chimera::ClientConfig c;
c.transport = chimera::TransportType::DoH;
c.use_hybrid_crypto = true;
c.encoding_strategy = chimera::EncodingStrategy::MULTI_RECORD;
c.use_compression = true;
c.noise_ratio = 0.1;
c.adaptive_transport = true;
```

## Steganography
- Prefer MULTI_RECORD, enable randomize_fragments, keep max_fragments
  reasonable

## Behavior
- Use timing_variance and BehavioralProfile to avoid patterns

## Operational tips
- Control domains and TLS trust
- Rotate binaries and vary timing patterns
- Never persist keys or secrets in logs

## Updates
- Keep libsodium/liboqs/OpenSSL/libcurl up to date

## Reporting
- Send vulnerabilities to maintainer email in README
