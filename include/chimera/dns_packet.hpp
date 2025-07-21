#pragma once

#include <string>
#include <vector>
#include <random>
#include <cstdint>

namespace chimera {

    enum class DnsType : uint16_t {
        A = 1,
        AAAA = 28,
        TXT = 16,
        CNAME = 5,
        MX = 15,
        NS = 2
    };

    enum class DnsClass : uint16_t {
        IN = 1,
        CH = 3,
        HS = 4
    };

    struct DnsQuestion {
        std::string name;
        DnsType type;
        DnsClass cls = DnsClass::IN;
    };

    struct DnsHeader {
        uint16_t id;
        uint16_t flags;
        uint16_t qdcount;
        uint16_t ancount;
        uint16_t nscount;
        uint16_t arcount;
    };

    struct DnsResourceRecord {
        std::string name;
        DnsType type;
        DnsClass cls;
        uint32_t ttl;
        std::vector<uint8_t> rdata;
    };

    class DnsPacketBuilder {
        static std::random_device rd;
        static std::mt19937 gen;

    public:
        static std::vector<uint8_t> build_query(const DnsQuestion& q, const std::string& payload = "");
        static std::vector<uint8_t> parse_response(const std::vector<uint8_t>& response, std::vector<DnsResourceRecord>& answers);

        static void print_packet_hex(const std::vector<uint8_t>& packet);
        static bool validate_domain_name(const std::string& domain);

    private:
        static void write_header(std::vector<uint8_t>& packet, const DnsHeader& hdr);
        static void write_question(std::vector<uint8_t>& packet, const DnsQuestion& q);
        static void write_domain_name(std::vector<uint8_t>& packet, const std::string& name);
        static void write_txt_data(std::vector<uint8_t>& packet, const std::string& data);
        static void write_uint16(std::vector<uint8_t>& packet, uint16_t value);

        static std::vector<std::string> split_domain(const std::string& domain);

        // DNS response processing
        static size_t read_domain_name(const std::vector<uint8_t>& data, size_t offset, std::string& out_name);
        static uint16_t read_uint16(const std::vector<uint8_t>& data, size_t offset);
        static uint32_t read_uint32(const std::vector<uint8_t>& data, size_t offset);
    };

} // namespace chimera
