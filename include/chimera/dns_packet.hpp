#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <random>

// DNS packet builder - teljesebb implementáció
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
        IN = 1, // Internet class
        CH = 3, // Chaos class
        HS = 4  // Hesiod class
    };

    struct DnsQuestion {
        std::string name;
        DnsType type;
        DnsClass cls = DnsClass::IN;
    };

    struct DnsHeader {
        uint16_t id;     // Query ID
        uint16_t flags;  // Flags (QR, Opcode, AA, TC, RD, RA, Z, RCODE)
        uint16_t qdcount; // Questions count
        uint16_t ancount; // Answers count
        uint16_t nscount; // Authority RRs count
        uint16_t arcount; // Additional RRs count
    };

    class DnsPacketBuilder {
        static std::random_device rd;
        static std::mt19937 gen;

    public:
        // DNS query építés
        static std::vector<uint8_t> build_query(const DnsQuestion& q, const std::string& payload = "");

        // DNS response parsing (még nincs teljes implementáció)
        static std::vector<std::string> parse_response(const std::vector<uint8_t>& response);

        // Segédfüggvények
        static void print_packet_hex(const std::vector<uint8_t>& packet);
        static bool validate_domain_name(const std::string& domain);

    private:
        static void write_header(std::vector<uint8_t>& packet, const DnsHeader& hdr);
        static void write_question(std::vector<uint8_t>& packet, const DnsQuestion& q);
        static void write_domain_name(std::vector<uint8_t>& packet, const std::string& name);
        static void write_txt_data(std::vector<uint8_t>& packet, const std::string& data);
        static void write_uint16(std::vector<uint8_t>& packet, uint16_t value);
        static std::vector<std::string> split_domain(const std::string& domain);
    };

} // namespace chimera
