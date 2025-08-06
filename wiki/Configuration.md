# Configuration Reference

## ClientConfig
```cpp
struct ClientConfig {
  std::string dns_server = "8.8.8.8";
  uint16_t dns_port = 53;
  std::string target_domain = "example.com";
  std::chrono::milliseconds timeout{5000};
  bool use_random_subdomains = true;
  bool use_hybrid_crypto = true;
  TransportType transport = TransportType::UDP;
  bool adaptive_transport = false;
  std::chrono::milliseconds timing_variance{100};
  BehavioralProfile behavioral_profile = BehavioralProfile::Normal;
  EncodingStrategy encoding_strategy = EncodingStrategy::MULTI_RECORD;
  bool use_compression = true;
  bool randomize_fragments = true;
  double noise_ratio = 0.1;
  size_t max_fragments = 10;
};
```

## Network
- dns_server: IPv4/IPv6/hostname of resolver
- dns_port: default 53
- target_domain: domain used for queries
- timeout: response wait duration

## Security
- use_random_subdomains: random subdomain per query
- use_hybrid_crypto: enable hybrid X25519 + ML-KEM

## Transport
- transport: UDP | DoH | DoT
- adaptive_transport: enable dynamic selection

## Behavioral mimicry
- timing_variance: jitter to vary timing
- behavioral_profile: Normal/WebBrowsing/Enterprise/Gaming/Random

## Steganography
- encoding_strategy: SINGLE_RECORD or MULTI_RECORD
- use_compression: compress payloads with zlib
- randomize_fragments: shuffle fragment order
- noise_ratio: 0.0..1.0 proportion of noise
- max_fragments: cap fragment count

## Examples
### Development
```cpp
chimera::ClientConfig dev() {
  chimera::ClientConfig c;
  c.transport = chimera::TransportType::UDP;
  c.use_random_subdomains = false;
  c.timeout = std::chrono::milliseconds(3000);
  return c;
}
```

### Production
```cpp
chimera::ClientConfig prod() {
  chimera::ClientConfig c;
  c.dns_server = "1.1.1.1";
  c.target_domain = "your-domain.com";
  c.transport = chimera::TransportType::DoH;
  c.timeout = std::chrono::milliseconds(10000);
  c.use_random_subdomains = true;
  c.use_hybrid_crypto = true;
  c.adaptive_transport = true;
  c.behavioral_profile = chimera::BehavioralProfile::WebBrowsing;
  c.timing_variance = std::chrono::milliseconds(200);
  return c;
}
```

### High-stealth
```cpp
chimera::ClientConfig stealth() {
  chimera::ClientConfig c;
  c.dns_server = "1.1.1.1";
  c.target_domain = "popular-site.com";
  c.transport = chimera::TransportType::DoH;
  c.timeout = std::chrono::milliseconds(15000);
  c.use_random_subdomains = true;
  c.use_hybrid_crypto = true;
  c.adaptive_transport = true;
  c.behavioral_profile = chimera::BehavioralProfile::Random;
  c.timing_variance = std::chrono::milliseconds(500);
  return c;
}
```

## Validation tips
- dns_server and target_domain must be non-empty
- timeout >= 1000 ms recommended

## Env vars (example pattern; implement if needed)
```bash
export CHIMERA_DNS_SERVER="1.1.1.1"
export CHIMERA_DOMAIN="your-domain.com"
export CHIMERA_TRANSPORT="doh|dot|udp"
export CHIMERA_TIMEOUT_MS="10000"
```

See also: Advanced-Features.md, API-Reference.md, Examples.md
