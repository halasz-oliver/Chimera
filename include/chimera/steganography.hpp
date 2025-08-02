#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <map>
#include "tl/expected.hpp"
#include "dns_packet.hpp"
#include "common.hpp"

namespace chimera {

    enum class SteganographyError {
        PayloadTooLarge,
        InvalidRecordType,
        EncodingError,
        DecodingError,
        FragmentationError
    };

    enum class EncodingStrategy {
        TXT_ONLY,           // Phase 1/2 compatibility - TXT records only
        MULTI_RECORD,       // Phase 3 - A, AAAA, TXT records
        DISTRIBUTED,        // Phase 3 - Advanced payload distribution
        HTTP2_BODY          // Phase 3 - HTTP/2 body encoding for DoH
    };

    struct EncodingConfig {
        EncodingStrategy strategy = EncodingStrategy::MULTI_RECORD;
        size_t max_txt_length = 255;      // Maximum TXT record length
        size_t max_fragments = 10;        // Maximum number of DNS fragments
        bool use_compression = true;       // Enable payload compression
        bool randomize_order = true;       // Randomize fragment order
        double noise_ratio = 0.1;         // Add noise records (0.0-1.0)
    };

    struct EncodedFragment {
        DnsType record_type;
        std::string domain;
        std::vector<uint8_t> encoded_data;
        uint32_t fragment_id;
        uint32_t total_fragments;
        std::vector<uint8_t> checksum;
    };

    struct DecodedPayload {
        std::vector<uint8_t> data;
        size_t original_size;
        std::chrono::milliseconds decode_time;
        std::vector<DnsType> used_record_types;
    };

    // IPv4 address encoding for A records (32-bit payload chunks)
    struct IPv4Encoding {
        static std::vector<uint8_t> encode_to_ipv4(const std::vector<uint8_t>& payload, size_t offset);
        static std::vector<uint8_t> decode_from_ipv4(const std::vector<uint8_t>& ipv4_bytes);
        static bool is_valid_steganographic_ip(const std::vector<uint8_t>& ip);
    };

    // IPv6 address encoding for AAAA records (128-bit payload chunks)
    struct IPv6Encoding {
        static std::vector<uint8_t> encode_to_ipv6(const std::vector<uint8_t>& payload, size_t offset);
        static std::vector<uint8_t> decode_from_ipv6(const std::vector<uint8_t>& ipv6_bytes);
        static bool is_valid_steganographic_ipv6(const std::vector<uint8_t>& ipv6);
    };

    // TXT record encoding (enhanced from Phase 1/2)
    struct TXTEncoding {
        static std::vector<std::string> encode_to_txt_fragments(const std::vector<uint8_t>& payload);
        static std::vector<uint8_t> decode_from_txt_fragments(const std::vector<std::string>& txt_records);
        static std::string create_steganographic_txt(const std::vector<uint8_t>& chunk, uint32_t fragment_id);
    };

    // HTTP/2 body encoding for DoH transport
    struct HTTP2Encoding {
        static std::vector<uint8_t> encode_to_http2_body(const std::vector<uint8_t>& payload);
        static std::vector<uint8_t> decode_from_http2_body(const std::vector<uint8_t>& http2_body);
        static std::map<std::string, std::string> create_steganographic_headers(const std::vector<uint8_t>& metadata);
    };

    class SteganographicEncoder {
        EncodingConfig config_;
        
    public:
        explicit SteganographicEncoder(EncodingConfig config = {}) : config_(std::move(config)) {}

        // Main encoding interface
        tl::expected<std::vector<EncodedFragment>, SteganographyError> 
        encode_payload(const std::vector<uint8_t>& payload, const std::string& base_domain) const;

        // Main decoding interface  
        tl::expected<DecodedPayload, SteganographyError>
        decode_fragments(const std::vector<EncodedFragment>& fragments) const;

        // Strategy-specific encoding
        tl::expected<std::vector<EncodedFragment>, SteganographyError>
        encode_txt_only(const std::vector<uint8_t>& payload, const std::string& base_domain) const;

        tl::expected<std::vector<EncodedFragment>, SteganographyError>
        encode_multi_record(const std::vector<uint8_t>& payload, const std::string& base_domain) const;

        tl::expected<std::vector<EncodedFragment>, SteganographyError>
        encode_distributed(const std::vector<uint8_t>& payload, const std::string& base_domain) const;

        tl::expected<std::vector<uint8_t>, SteganographyError>
        encode_http2_body(const std::vector<uint8_t>& payload) const;

        // Fragment management
        static std::vector<EncodedFragment> add_noise_fragments(
            std::vector<EncodedFragment> fragments, 
            const std::string& base_domain, 
            double noise_ratio
        );

        static std::vector<EncodedFragment> randomize_fragment_order(std::vector<EncodedFragment> fragments);

        // Utility functions
        [[nodiscard]] const EncodingConfig& get_config() const { return config_; }
        void update_config(EncodingConfig new_config) { config_ = std::move(new_config); }

        // Capacity estimation
        static size_t estimate_capacity(DnsType record_type, size_t max_fragments = 10);
        static size_t estimate_total_capacity(const EncodingConfig& config);

    private:
        std::vector<uint8_t> compress_payload(const std::vector<uint8_t>& payload) const;
        std::vector<uint8_t> decompress_payload(const std::vector<uint8_t>& compressed) const;
        
        std::vector<uint8_t> calculate_checksum(const std::vector<uint8_t>& data) const;
        bool verify_checksum(const std::vector<uint8_t>& data, const std::vector<uint8_t>& checksum) const;

        std::string generate_steganographic_subdomain(uint32_t fragment_id, DnsType record_type) const;
    };

    // Response parsing and extraction for bidirectional communication
    class SteganographicExtractor {
    public:
        // Extract hidden data from DNS responses
        static tl::expected<std::vector<uint8_t>, SteganographyError>
        extract_from_dns_response(const std::vector<DnsResourceRecord>& records);

        // Extract hidden data from HTTP/2 DoH responses
        static tl::expected<std::vector<uint8_t>, SteganographyError>
        extract_from_http2_response(const std::vector<uint8_t>& http2_response);

        // Pattern detection for steganographic content
        static bool detect_steganographic_pattern(const DnsResourceRecord& record);
        static bool detect_steganographic_http2(const std::vector<uint8_t>& http2_body);

        // Multi-record reconstruction
        static tl::expected<std::vector<uint8_t>, SteganographyError>
        reconstruct_from_fragments(const std::vector<EncodedFragment>& fragments);

    private:
        static std::vector<EncodedFragment> sort_fragments_by_id(std::vector<EncodedFragment> fragments);
        static bool validate_fragment_sequence(const std::vector<EncodedFragment>& fragments);
    };

} // namespace chimera