#include "chimera/steganography.hpp"
#include "chimera/base64.hpp"
#include <algorithm>
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <zlib.h>

namespace chimera {

    // IPv4 encoding implementation
    std::vector<uint8_t> IPv4Encoding::encode_to_ipv4(const std::vector<uint8_t>& payload, size_t offset) {
        std::vector<uint8_t> ipv4(4, 0);
        
        // Encode up to 4 bytes of payload into IPv4 address
        for (size_t i = 0; i < 4 && (offset + i) < payload.size(); ++i) {
            ipv4[i] = payload[offset + i];
        }
        
        // If payload is shorter, use padding that looks like valid IP ranges
        if (offset + 4 > payload.size()) {
            // Use common private IP ranges for padding
            if (ipv4[0] == 0) ipv4[0] = 192;  // 192.x.x.x
            if (ipv4[1] == 0) ipv4[1] = 168;  // 192.168.x.x
        }
        
        return ipv4;
    }

    std::vector<uint8_t> IPv4Encoding::decode_from_ipv4(const std::vector<uint8_t>& ipv4_bytes) {
        if (ipv4_bytes.size() != 4) {
            return {};
        }
        return ipv4_bytes;
    }

    bool IPv4Encoding::is_valid_steganographic_ip(const std::vector<uint8_t>& ip) {
        if (ip.size() != 4) return false;
        
        // Check if it's in private ranges (more likely to be steganographic)
        return (ip[0] == 192 && ip[1] == 168) ||  // 192.168.x.x
               (ip[0] == 10) ||                   // 10.x.x.x
               (ip[0] == 172 && ip[1] >= 16 && ip[1] <= 31); // 172.16-31.x.x
    }

    // IPv6 encoding implementation
    std::vector<uint8_t> IPv6Encoding::encode_to_ipv6(const std::vector<uint8_t>& payload, size_t offset) {
        std::vector<uint8_t> ipv6(16, 0);
        
        // Encode up to 16 bytes of payload into IPv6 address
        for (size_t i = 0; i < 16 && (offset + i) < payload.size(); ++i) {
            ipv6[i] = payload[offset + i];
        }
        
        // Use IPv6 local prefix for padding (fe80::/10)
        if (offset + 16 > payload.size()) {
            if (ipv6[0] == 0) ipv6[0] = 0xfe;
            if (ipv6[1] == 0) ipv6[1] = 0x80;
        }
        
        return ipv6;
    }

    std::vector<uint8_t> IPv6Encoding::decode_from_ipv6(const std::vector<uint8_t>& ipv6_bytes) {
        if (ipv6_bytes.size() != 16) {
            return {};
        }
        return ipv6_bytes;
    }

    bool IPv6Encoding::is_valid_steganographic_ipv6(const std::vector<uint8_t>& ipv6) {
        if (ipv6.size() != 16) return false;
        
        // Check for local IPv6 prefixes (more likely to be steganographic)
        return (ipv6[0] == 0xfe && (ipv6[1] & 0xc0) == 0x80) ||  // fe80::/10 (link-local)
               (ipv6[0] == 0xfc || ipv6[0] == 0xfd);              // fc00::/7 (unique local)
    }

    // TXT encoding implementation
    std::vector<std::string> TXTEncoding::encode_to_txt_fragments(const std::vector<uint8_t>& payload) {
        std::vector<std::string> fragments;
        
        // Base64 encode the payload
        std::string encoded = Base64::encode(std::string(payload.begin(), payload.end()));
        
        // Split into TXT-record sized chunks (max 255 bytes per TXT record)
        // Ensure chunks are multiples of 4 for valid base64
        const size_t max_chunk_size = 200; // Leave room for fragment metadata
        const size_t chunk_size = (max_chunk_size / 4) * 4; // Round down to multiple of 4
        
        for (size_t i = 0; i < encoded.length(); i += chunk_size) {
            size_t actual_chunk_size = std::min(chunk_size, encoded.length() - i);
            std::string chunk = encoded.substr(i, actual_chunk_size);
            
            // Pad the chunk if it's the last one and not a multiple of 4
            if (i + actual_chunk_size >= encoded.length() && chunk.length() % 4 != 0) {
                while (chunk.length() % 4 != 0) {
                    chunk += '=';
                }
            }
            
            // Add fragment metadata
            uint32_t fragment_id = static_cast<uint32_t>(fragments.size());
            fragments.push_back(create_steganographic_txt(
                std::vector<uint8_t>(chunk.begin(), chunk.end()), fragment_id));
        }
        
        return fragments;
    }

    std::vector<uint8_t> TXTEncoding::decode_from_txt_fragments(const std::vector<std::string>& txt_records) {
        std::string combined;
        
        for (const auto& txt : txt_records) {
            // Extract the base64 part (skip metadata prefix if present)
            // Look for the pattern "frag=<hex>=" to find where the data starts
            size_t frag_pos = txt.find("frag=");
            if (frag_pos != std::string::npos) {
                // Find the '=' after the fragment ID
                size_t data_start = txt.find('=', frag_pos + 5); // Skip "frag="
                if (data_start != std::string::npos) {
                    combined += txt.substr(data_start + 1);
                } else {
                    // Fallback: use everything after "frag="
                    size_t equals_pos = txt.find('=', frag_pos);
                    if (equals_pos != std::string::npos) {
                        combined += txt.substr(equals_pos + 1);
                    }
                }
            } else {
                // No fragment metadata, assume entire string is base64
                combined += txt;
            }
        }
        
        auto decoded = Base64::decode(combined);
        return std::vector<uint8_t>(decoded.begin(), decoded.end());
    }

    std::string TXTEncoding::create_steganographic_txt(const std::vector<uint8_t>& chunk, uint32_t fragment_id) {
        std::ostringstream oss;
        oss << "v=spf1 include:_spf.google.com ~all; frag=" << std::hex << fragment_id << "=";
        // The chunk is already base64-encoded data, so convert it back to string
        oss << std::string(chunk.begin(), chunk.end());
        return oss.str();
    }

    // HTTP/2 encoding implementation
    std::vector<uint8_t> HTTP2Encoding::encode_to_http2_body(const std::vector<uint8_t>& payload) {
        // Create a DNS-over-HTTPS POST body that contains steganographic data
        std::vector<uint8_t> http2_body;
        
        // Standard DoH header
        std::string content_type = "application/dns-message";
        
        // Embed payload in HTTP/2 padding or custom headers
        // For now, embed in the DNS message itself with additional padding
        http2_body.insert(http2_body.end(), payload.begin(), payload.end());
        
        // Add padding to make it look like a legitimate DNS query
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        
        size_t padding_size = 32 + (gen() % 64); // Random padding
        for (size_t i = 0; i < padding_size; ++i) {
            http2_body.push_back(static_cast<uint8_t>(dis(gen)));
        }
        
        return http2_body;
    }

    std::vector<uint8_t> HTTP2Encoding::decode_from_http2_body(const std::vector<uint8_t>& http2_body) {
        // For this implementation, assume the payload is at the beginning
        // In a real implementation, this would parse HTTP/2 frames
        if (http2_body.size() < 32) {
            return {};
        }
        
        // Extract the first part as payload (before padding)
        // This is a simplified implementation
        return std::vector<uint8_t>(http2_body.begin(), http2_body.begin() + 32);
    }

    std::map<std::string, std::string> HTTP2Encoding::create_steganographic_headers(const std::vector<uint8_t>& metadata) {
        std::map<std::string, std::string> headers;
        
        // Embed metadata in custom headers that look legitimate
        headers["X-Request-ID"] = Base64::encode(std::string(metadata.begin(), metadata.end()));
        headers["X-Forwarded-For"] = "203.0.113.1"; // Documentation IP
        headers["User-Agent"] = "Mozilla/5.0 (compatible; DNS-Client/1.0)";
        
        return headers;
    }

    // Main encoder implementation
    tl::expected<std::vector<EncodedFragment>, SteganographyError> 
    SteganographicEncoder::encode_payload(const std::vector<uint8_t>& payload, const std::string& base_domain) const {
        
        if (payload.empty()) {
            return tl::unexpected(SteganographyError::PayloadTooLarge);
        }

        // Compress payload if enabled
        std::vector<uint8_t> processed_payload = config_.use_compression ? 
            compress_payload(payload) : payload;

        // Route to appropriate encoding strategy
        switch (config_.strategy) {
            case EncodingStrategy::TXT_ONLY:
                return encode_txt_only(processed_payload, base_domain);
            case EncodingStrategy::MULTI_RECORD:
                return encode_multi_record(processed_payload, base_domain);
            case EncodingStrategy::DISTRIBUTED:
                return encode_distributed(processed_payload, base_domain);
            case EncodingStrategy::HTTP2_BODY:
                // HTTP2 encoding returns different format, handle separately
                break;
        }

        return tl::unexpected(SteganographyError::EncodingError);
    }

    tl::expected<std::vector<EncodedFragment>, SteganographyError>
    SteganographicEncoder::encode_txt_only(const std::vector<uint8_t>& payload, const std::string& base_domain) const {
        
        std::vector<EncodedFragment> fragments;
        auto txt_fragments = TXTEncoding::encode_to_txt_fragments(payload);
        
        for (size_t i = 0; i < txt_fragments.size(); ++i) {
            EncodedFragment fragment;
            fragment.record_type = DnsType::TXT;
            fragment.domain = generate_steganographic_subdomain(static_cast<uint32_t>(i), DnsType::TXT) + "." + base_domain;
            fragment.encoded_data = std::vector<uint8_t>(txt_fragments[i].begin(), txt_fragments[i].end());
            fragment.fragment_id = static_cast<uint32_t>(i);
            fragment.total_fragments = static_cast<uint32_t>(txt_fragments.size());
            fragment.checksum = calculate_checksum(fragment.encoded_data);
            
            fragments.push_back(std::move(fragment));
        }
        
        return fragments;
    }

    tl::expected<std::vector<EncodedFragment>, SteganographyError>
    SteganographicEncoder::encode_multi_record(const std::vector<uint8_t>& payload, const std::string& base_domain) const {
        
        std::vector<EncodedFragment> fragments;
        size_t offset = 0;
        uint32_t fragment_id = 0;
        
        // Distribute payload across different record types
        while (offset < payload.size()) {
            // Cycle through A, AAAA, TXT records
            DnsType record_type;
            size_t chunk_size;
            
            switch (fragment_id % 3) {
                case 0: // A record - 4 bytes
                    record_type = DnsType::A;
                    chunk_size = 4;
                    break;
                case 1: // AAAA record - 16 bytes
                    record_type = DnsType::AAAA;
                    chunk_size = 16;
                    break;
                case 2: // TXT record - larger chunks
                    record_type = DnsType::TXT;
                    chunk_size = std::min(static_cast<size_t>(200), payload.size() - offset);
                    break;
                default:
                    record_type = DnsType::TXT;
                    chunk_size = 200;
                    break;
            }
            
            chunk_size = std::min(chunk_size, payload.size() - offset);
            
            EncodedFragment fragment;
            fragment.record_type = record_type;
            fragment.domain = generate_steganographic_subdomain(fragment_id, record_type) + "." + base_domain;
            fragment.fragment_id = fragment_id;
            
            // Encode based on record type
            std::vector<uint8_t> chunk(payload.begin() + offset, payload.begin() + offset + chunk_size);
            
            switch (record_type) {
                case DnsType::A:
                    fragment.encoded_data = IPv4Encoding::encode_to_ipv4(payload, offset);
                    break;
                case DnsType::AAAA:
                    fragment.encoded_data = IPv6Encoding::encode_to_ipv6(payload, offset);
                    break;
                case DnsType::TXT:
                    {
                        // Base64 encode the raw chunk first
                        std::string encoded_chunk = Base64::encode(std::string(chunk.begin(), chunk.end()));
                        std::string txt = TXTEncoding::create_steganographic_txt(
                            std::vector<uint8_t>(encoded_chunk.begin(), encoded_chunk.end()), fragment_id);
                        fragment.encoded_data = std::vector<uint8_t>(txt.begin(), txt.end());
                    }
                    break;
                default:
                    break;
            }
            
            fragment.checksum = calculate_checksum(fragment.encoded_data);
            fragments.push_back(std::move(fragment));
            
            offset += chunk_size;
            fragment_id++;
            
            if (fragment_id >= config_.max_fragments) {
                break;
            }
        }
        
        // Set total fragments count
        for (auto& frag : fragments) {
            frag.total_fragments = static_cast<uint32_t>(fragments.size());
        }
        
        // Add noise fragments if configured
        if (config_.noise_ratio > 0.0) {
            fragments = add_noise_fragments(std::move(fragments), base_domain, config_.noise_ratio);
        }
        
        // Randomize order if configured
        if (config_.randomize_order) {
            fragments = randomize_fragment_order(std::move(fragments));
        }
        
        return fragments;
    }

    tl::expected<std::vector<EncodedFragment>, SteganographyError>
    SteganographicEncoder::encode_distributed(const std::vector<uint8_t>& payload, const std::string& base_domain) const {
        
        // Advanced distribution strategy - spread payload more evenly
        auto result = encode_multi_record(payload, base_domain);
        if (!result) {
            return result;
        }
        
        auto fragments = result.value();
        
        // Additional distribution logic - interleave different record types
        std::stable_sort(fragments.begin(), fragments.end(), 
            [](const EncodedFragment& a, const EncodedFragment& b) {
                return static_cast<int>(a.record_type) < static_cast<int>(b.record_type);
            });
        
        return fragments;
    }

    tl::expected<std::vector<uint8_t>, SteganographyError>
    SteganographicEncoder::encode_http2_body(const std::vector<uint8_t>& payload) const {
        return HTTP2Encoding::encode_to_http2_body(payload);
    }

    // Utility functions
    std::vector<EncodedFragment> SteganographicEncoder::add_noise_fragments(
        std::vector<EncodedFragment> fragments, 
        const std::string& base_domain, 
        double noise_ratio) {
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        std::uniform_int_distribution<> type_dis(1, 16); // Random DNS types
        
        size_t noise_count = static_cast<size_t>(fragments.size() * noise_ratio);
        
        for (size_t i = 0; i < noise_count; ++i) {
            EncodedFragment noise_fragment;
            noise_fragment.record_type = static_cast<DnsType>(type_dis(gen));
            noise_fragment.domain = "noise" + std::to_string(i) + "." + base_domain;
            noise_fragment.fragment_id = 0xFFFFFFFF; // Mark as noise
            noise_fragment.total_fragments = 0;
            
            // Generate random noise data
            std::uniform_int_distribution<> byte_dis(0, 255);
            for (int j = 0; j < 32; ++j) {
                noise_fragment.encoded_data.push_back(static_cast<uint8_t>(byte_dis(gen)));
            }
            
            fragments.push_back(std::move(noise_fragment));
        }
        
        return fragments;
    }

    std::vector<EncodedFragment> SteganographicEncoder::randomize_fragment_order(std::vector<EncodedFragment> fragments) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(fragments.begin(), fragments.end(), gen);
        return fragments;
    }

    size_t SteganographicEncoder::estimate_capacity(DnsType record_type, size_t max_fragments) {
        switch (record_type) {
            case DnsType::A:
                return 4 * max_fragments;
            case DnsType::AAAA:
                return 16 * max_fragments;
            case DnsType::TXT:
                return 200 * max_fragments; // Conservative estimate
            default:
                return 0;
        }
    }

    size_t SteganographicEncoder::estimate_total_capacity(const EncodingConfig& config) {
        switch (config.strategy) {
            case EncodingStrategy::TXT_ONLY:
                return estimate_capacity(DnsType::TXT, config.max_fragments);
            case EncodingStrategy::MULTI_RECORD:
            case EncodingStrategy::DISTRIBUTED:
                // Mix of A, AAAA, TXT
                return (estimate_capacity(DnsType::A, config.max_fragments / 3) +
                        estimate_capacity(DnsType::AAAA, config.max_fragments / 3) +
                        estimate_capacity(DnsType::TXT, config.max_fragments / 3));
            case EncodingStrategy::HTTP2_BODY:
                return 1024; // Estimate for HTTP/2 body
        }
        return 0;
    }

    // Private helper functions
    std::vector<uint8_t> SteganographicEncoder::compress_payload(const std::vector<uint8_t>& payload) const {
        // Simple zlib compression
        z_stream zs;
        memset(&zs, 0, sizeof(zs));
        
        if (deflateInit(&zs, Z_DEFAULT_COMPRESSION) != Z_OK) {
            return payload; // Return original on compression failure
        }
        
        zs.next_in = const_cast<Bytef*>(payload.data());
        zs.avail_in = static_cast<uInt>(payload.size());
        
        std::vector<uint8_t> compressed;
        compressed.resize(payload.size() + 32); // Initial size estimate
        
        zs.next_out = compressed.data();
        zs.avail_out = static_cast<uInt>(compressed.size());
        
        int ret = deflate(&zs, Z_FINISH);
        deflateEnd(&zs);
        
        if (ret == Z_STREAM_END) {
            compressed.resize(zs.total_out);
            return compressed;
        }
        
        return payload; // Return original on compression failure
    }

    std::vector<uint8_t> SteganographicEncoder::decompress_payload(const std::vector<uint8_t>& compressed) const {
        // Simple zlib decompression
        z_stream zs;
        memset(&zs, 0, sizeof(zs));
        
        if (inflateInit(&zs) != Z_OK) {
            return compressed; // Return original on decompression failure
        }
        
        zs.next_in = const_cast<Bytef*>(compressed.data());
        zs.avail_in = static_cast<uInt>(compressed.size());
        
        std::vector<uint8_t> decompressed;
        decompressed.resize(compressed.size() * 4); // Initial size estimate
        
        zs.next_out = decompressed.data();
        zs.avail_out = static_cast<uInt>(decompressed.size());
        
        int ret = inflate(&zs, Z_FINISH);
        inflateEnd(&zs);
        
        if (ret == Z_STREAM_END) {
            decompressed.resize(zs.total_out);
            return decompressed;
        }
        
        return compressed; // Return original on decompression failure
    }

    std::vector<uint8_t> SteganographicEncoder::calculate_checksum(const std::vector<uint8_t>& data) const {
        // Simple CRC32 checksum
        uint32_t crc = 0xFFFFFFFF;
        for (uint8_t byte : data) {
            crc ^= byte;
            for (int i = 0; i < 8; ++i) {
                if (crc & 1) {
                    crc = (crc >> 1) ^ 0xEDB88320;
                } else {
                    crc >>= 1;
                }
            }
        }
        crc ^= 0xFFFFFFFF;
        
        std::vector<uint8_t> checksum(4);
        checksum[0] = static_cast<uint8_t>(crc & 0xFF);
        checksum[1] = static_cast<uint8_t>((crc >> 8) & 0xFF);
        checksum[2] = static_cast<uint8_t>((crc >> 16) & 0xFF);
        checksum[3] = static_cast<uint8_t>((crc >> 24) & 0xFF);
        
        return checksum;
    }

    bool SteganographicEncoder::verify_checksum(const std::vector<uint8_t>& data, const std::vector<uint8_t>& checksum) const {
        auto calculated = calculate_checksum(data);
        return calculated == checksum;
    }

    std::string SteganographicEncoder::generate_steganographic_subdomain(uint32_t fragment_id, DnsType record_type) const {
        std::ostringstream oss;
        
        // Create subdomain that looks legitimate but encodes metadata
        switch (record_type) {
            case DnsType::A:
                oss << "www" << std::hex << fragment_id;
                break;
            case DnsType::AAAA:
                oss << "ipv6-" << std::hex << fragment_id;
                break;
            case DnsType::TXT:
                oss << "mail" << std::hex << fragment_id;
                break;
            default:
                oss << "srv" << std::hex << fragment_id;
                break;
        }
        
        return oss.str();
    }

    // Decoder implementation
    tl::expected<DecodedPayload, SteganographyError>
    SteganographicEncoder::decode_fragments(const std::vector<EncodedFragment>& fragments) const {
        
        if (fragments.empty()) {
            return tl::unexpected(SteganographyError::DecodingError);
        }

        auto start_time = std::chrono::high_resolution_clock::now();

        // Sort fragments by ID
        auto sorted_fragments = fragments;
        std::sort(sorted_fragments.begin(), sorted_fragments.end(),
            [](const EncodedFragment& a, const EncodedFragment& b) {
                return a.fragment_id < b.fragment_id;
            });

        // Filter out noise fragments
        sorted_fragments.erase(
            std::remove_if(sorted_fragments.begin(), sorted_fragments.end(),
                [](const EncodedFragment& frag) {
                    return frag.fragment_id == 0xFFFFFFFF; // Noise marker
                }),
            sorted_fragments.end());

        // Reconstruct payload
        std::vector<uint8_t> reconstructed_data;
        std::vector<DnsType> used_types;

        for (const auto& fragment : sorted_fragments) {
            // Verify checksum
            if (!verify_checksum(fragment.encoded_data, fragment.checksum)) {
                continue; // Skip corrupted fragments
            }

            used_types.push_back(fragment.record_type);

            // Decode based on record type
            switch (fragment.record_type) {
                case DnsType::A:
                    {
                        auto decoded = IPv4Encoding::decode_from_ipv4(fragment.encoded_data);
                        reconstructed_data.insert(reconstructed_data.end(), decoded.begin(), decoded.end());
                    }
                    break;
                case DnsType::AAAA:
                    {
                        auto decoded = IPv6Encoding::decode_from_ipv6(fragment.encoded_data);
                        reconstructed_data.insert(reconstructed_data.end(), decoded.begin(), decoded.end());
                    }
                    break;
                case DnsType::TXT:
                    {
                        std::string txt_data(fragment.encoded_data.begin(), fragment.encoded_data.end());
                        auto decoded = TXTEncoding::decode_from_txt_fragments({txt_data});
                        reconstructed_data.insert(reconstructed_data.end(), decoded.begin(), decoded.end());
                    }
                    break;
                default:
                    break;
            }
        }

        // Decompress if compression was used
        if (config_.use_compression) {
            reconstructed_data = decompress_payload(reconstructed_data);
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto decode_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        DecodedPayload result;
        result.data = std::move(reconstructed_data);
        result.original_size = result.data.size();
        result.decode_time = decode_time;
        result.used_record_types = std::move(used_types);

        return result;
    }

    // Extractor implementation
    tl::expected<std::vector<uint8_t>, SteganographyError>
    SteganographicExtractor::extract_from_dns_response(const std::vector<DnsResourceRecord>& records) {
        
        std::vector<EncodedFragment> fragments;
        
        for (const auto& record : records) {
            if (!detect_steganographic_pattern(record)) {
                continue;
            }
            
            EncodedFragment fragment;
            fragment.record_type = record.type;
            fragment.domain = record.name;
            fragment.encoded_data = record.rdata;
            
            // Extract fragment ID from domain or data
            // This is a simplified implementation
            fragment.fragment_id = static_cast<uint32_t>(fragments.size());
            fragment.total_fragments = static_cast<uint32_t>(records.size());
            
            fragments.push_back(std::move(fragment));
        }
        
        return reconstruct_from_fragments(fragments);
    }

    tl::expected<std::vector<uint8_t>, SteganographyError>
    SteganographicExtractor::extract_from_http2_response(const std::vector<uint8_t>& http2_response) {
        return HTTP2Encoding::decode_from_http2_body(http2_response);
    }

    bool SteganographicExtractor::detect_steganographic_pattern(const DnsResourceRecord& record) {
        // Look for patterns that indicate steganographic content
        switch (record.type) {
            case DnsType::A:
                return IPv4Encoding::is_valid_steganographic_ip(record.rdata);
            case DnsType::AAAA:
                return IPv6Encoding::is_valid_steganographic_ipv6(record.rdata);
            case DnsType::TXT:
                {
                    std::string txt_data(record.rdata.begin(), record.rdata.end());
                    return txt_data.find("frag=") != std::string::npos ||
                           txt_data.find("v=spf1") != std::string::npos;
                }
            default:
                return false;
        }
    }

    bool SteganographicExtractor::detect_steganographic_http2(const std::vector<uint8_t>& http2_body) {
        // Simple heuristic - check for unusual padding or patterns
        return http2_body.size() > 64; // Larger than typical DNS queries
    }

    tl::expected<std::vector<uint8_t>, SteganographyError>
    SteganographicExtractor::reconstruct_from_fragments(const std::vector<EncodedFragment>& fragments) {
        
        auto sorted_fragments = sort_fragments_by_id(const_cast<std::vector<EncodedFragment>&>(fragments));
        
        if (!validate_fragment_sequence(sorted_fragments)) {
            return tl::unexpected(SteganographyError::FragmentationError);
        }
        
        std::vector<uint8_t> reconstructed;
        
        for (const auto& fragment : sorted_fragments) {
            reconstructed.insert(reconstructed.end(), 
                                fragment.encoded_data.begin(), 
                                fragment.encoded_data.end());
        }
        
        return reconstructed;
    }

    std::vector<EncodedFragment> SteganographicExtractor::sort_fragments_by_id(std::vector<EncodedFragment> fragments) {
        std::sort(fragments.begin(), fragments.end(),
            [](const EncodedFragment& a, const EncodedFragment& b) {
                return a.fragment_id < b.fragment_id;
            });
        return fragments;
    }

    bool SteganographicExtractor::validate_fragment_sequence(const std::vector<EncodedFragment>& fragments) {
        if (fragments.empty()) return false;
        
        // Check for sequential fragment IDs
        for (size_t i = 0; i < fragments.size(); ++i) {
            if (fragments[i].fragment_id != i) {
                return false; // Missing fragment
            }
        }
        
        return true;
    }

} // namespace chimera