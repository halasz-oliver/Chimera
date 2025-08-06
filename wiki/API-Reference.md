# API Reference

## Core classes
ChimeraClient (include/chimera/client.hpp)
```cpp
class ChimeraClient {
public:
  explicit ChimeraClient(ClientConfig config);
  tl::expected<SendResult, ChimeraError>
  send_text(const std::string& message) const;
  tl::expected<SendResult, ChimeraError>
  send_data(const std::vector<uint8_t>& data) const;
  tl::expected<SendResult, ChimeraError>
  send_file(const std::string& file_path) const;
  tl::expected<SendResult, ChimeraError>
  send_multi_record(const std::vector<uint8_t>& data) const;
  tl::expected<std::vector<uint8_t>, ChimeraError>
  receive_data(const std::string& query_domain) const;
  size_t estimate_capacity() const;
  const ClientConfig& get_config() const;
  void update_config(ClientConfig new_config);
  tl::expected<std::chrono::milliseconds, ChimeraError>
  ping_dns_server() const;
};
```

## Config
```cpp
struct ClientConfig {
  // see Configuration.md for full fields matching header
};
```

## Enums
- TransportType: UDP, DoH, DoT
- EncodingStrategy: SINGLE_RECORD, MULTI_RECORD
- BehavioralProfile: Normal, WebBrowsing, Enterprise, Gaming, Random
- ChimeraError: NetworkError, ConfigError, EncodingError, DecodingError,
  TimeoutError, DnsError, CryptoError

## Results
```cpp
struct SendResult {
  size_t bytes_sent;
  std::chrono::milliseconds latency;
  std::string used_domain;
  std::vector<DnsType> used_record_types;
  size_t fragments_sent;
  EncodingStrategy encoding_used;
  bool compression_used;
};
```

## tl::expected
- Success: access via * or ->
- Error: result.error() returns ChimeraError

## Examples
```cpp
chimera::ClientConfig c;
c.dns_server = "1.1.1.1";
c.target_domain = "example.com";
c.transport = chimera::TransportType::DoH;
chimera::ChimeraClient cl(c);
auto r = cl.send_text("hello");
```

## Notes
- Match signatures in include headers; avoid assuming extra APIs
- See README and wiki pages for build/test and usage patterns
