# Basic Usage

## Create a client and send text
```cpp
#include "chimera/client.hpp"
int main() {
  chimera::ClientConfig c;
  c.dns_server = "8.8.8.8";
  c.target_domain = "example.com";
  chimera::ChimeraClient cl(c);
  auto r = cl.send_text("Hello");
  if (!r) return 1;
}
```

## CLI demo
```bash
# built at build/chimera_demo
build/chimera_demo "Your secret message"
```

## Key config
```cpp
chimera::ClientConfig c;
c.dns_server = "8.8.8.8";
c.dns_port = 53;
c.target_domain = "example.com";
c.timeout = std::chrono::milliseconds(5000);
c.transport = chimera::TransportType::UDP; // or DoH/DoT
c.use_random_subdomains = true;
c.use_hybrid_crypto = true;
// behavioral mimicry
auto ms = std::chrono::milliseconds(100);
c.adaptive_transport = false;
c.timing_variance = ms;
c.behavioral_profile = chimera::BehavioralProfile::Normal;
// steganography options
c.encoding_strategy = chimera::EncodingStrategy::MULTI_RECORD;
c.use_compression = true;
c.randomize_fragments = true;
c.noise_ratio = 0.1;
c.max_fragments = 10;
```

## Error handling
```cpp
auto res = cl.send_text("msg");
if (!res) {
  // inspect error and retry/backoff
}
```

## Ping and basic check
```cpp
auto ping = cl.ping_dns_server();
if (ping) {
  // ok
}
```

## Run tests
```bash
cmake --build build --target run_tests
# or
build/chimera_test --all
```

## Tips
- Start with UDP for local testing, then switch to DoH/DoT for stealth
- Keep messages small; use compression for larger payloads
- Use the configuration wizard for validated interactive setup
