#include "chimera/dns_packet.hpp"
#include <iostream>
#include <stdexcept>

namespace chimera {

// Static member definitions
std::random_device DnsPacketBuilder::rd;
std::mt19937 DnsPacketBuilder::gen(DnsPacketBuilder::rd());

std::vector<uint8_t> DnsPacketBuilder::build_query(const DnsQuestion& q, const std::string& payload) {
    std::vector<uint8_t> packet;

    try {
        // Header építés
        DnsHeader hdr{};
        hdr.id = gen() & 0xFFFF;  // random ID minden querynél
        hdr.flags = 0x0100;      // standard query with recursion desired
        hdr.qdcount = 1;         // egy kérdés
        hdr.ancount = 0;         // nincs válasz (query)
        hdr.nscount = 0;         // nincs authority
        hdr.arcount = 0;         // nincs additional

        write_header(packet, hdr);
        write_question(packet, q);

        // Ha van payload és TXT record, akkor hozzáadjuk
        // TODO: ez nem szabványos DNS, de teszt célokra jó
        if (!payload.empty() && q.type == DnsType::TXT) {
            write_txt_data(packet, payload);
        }

        std::cout << "DNS packet építve: " << packet.size() << " byte, ID="
                  << std::hex << hdr.id << std::dec << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "DNS packet építési hiba: " << e.what() << std::endl;
        throw;
    }

    return packet;
}

// TODO: DNS response parser implementáció
std::vector<uint8_t> DnsPacketBuilder::parse_response(const std::vector<uint8_t>& response) {
    if (response.size() < 12) {
        throw std::runtime_error("DNS response túl rövid");
    }

    // Egyelőre csak a header-t olvassuk
    uint16_t id = (response[0] << 8) | response[1];
    uint16_t flags = (response[2] << 8) | response[3];
    uint16_t qdcount = (response[4] << 8) | response[5];
    uint16_t ancount = (response[6] << 8) | response[7];

    std::cout << "DNS response: ID=" << std::hex << id
              << ", flags=" << flags
              << ", questions=" << std::dec << qdcount
              << ", answers=" << ancount << std::endl;

    // TODO: teljes parsing implementáció
    return {}; // placeholder
}

void DnsPacketBuilder::write_header(std::vector<uint8_t>& packet, const DnsHeader& hdr) {
    write_uint16(packet, hdr.id);
    write_uint16(packet, hdr.flags);
    write_uint16(packet, hdr.qdcount);
    write_uint16(packet, hdr.ancount);
    write_uint16(packet, hdr.nscount);
    write_uint16(packet, hdr.arcount);
}

void DnsPacketBuilder::write_question(std::vector<uint8_t>& packet, const DnsQuestion& q) {
    write_domain_name(packet, q.name);
    write_uint16(packet, static_cast<uint16_t>(q.type));
    write_uint16(packet, static_cast<uint16_t>(q.cls));
}

void DnsPacketBuilder::write_domain_name(std::vector<uint8_t>& packet, const std::string& name) {
    if (name.empty()) {
        packet.push_back(0); // root domain
        return;
    }

    auto labels = split_domain(name);
    for (const auto& label : labels) {
        if (label.empty()) continue; // skip empty labels

        if (label.size() > 63) {
            throw std::runtime_error("DNS label túl hosszú: " + label + " (" + std::to_string(label.size()) + " > 63)");
        }

        packet.push_back(static_cast<uint8_t>(label.size()));
        packet.insert(packet.end(), label.begin(), label.end());
    }
    packet.push_back(0); // null terminator
}

void DnsPacketBuilder::write_txt_data(std::vector<uint8_t>& packet, const std::string& data) {
    // Ez nem szabványos DNS format, csak demo célokra
    // Valódi implementációban ez a válasz részben lenne
    if (data.size() > 255) {
        throw std::runtime_error("TXT adat túl hosszú egy chunk-hoz: " + std::to_string(data.size()));
    }

    // TXT record formátum: length byte + data
    packet.push_back(static_cast<uint8_t>(data.size()));
    packet.insert(packet.end(), data.begin(), data.end());

    std::cout << "TXT payload hozzáadva: " << data.size() << " byte" << std::endl;
}

void DnsPacketBuilder::write_uint16(std::vector<uint8_t>& packet, uint16_t value) {
    // DNS big-endian formátum
    packet.push_back((value >> 8) & 0xFF);
    packet.push_back(value & 0xFF);
}

std::vector<std::string> DnsPacketBuilder::split_domain(const std::string& domain) {
    std::vector<std::string> labels;
    std::string current;

    for (char c : domain) {
        if (c == '.') {
            if (!current.empty()) {
                labels.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }

    // Utolsó label hozzáadása
    if (!current.empty()) {
        labels.push_back(current);
    }

    return labels;
}

// Utility függvények a debug-hoz
void DnsPacketBuilder::print_packet_hex(const std::vector<uint8_t>& packet) {
    std::cout << "DNS packet hex dump:" << std::endl;
    for (size_t i = 0; i < packet.size(); ++i) {
        if (i % 16 == 0) std::cout << std::endl;
        std::cout << std::hex << std::setfill('0') << std::setw(2)
                  << static_cast<int>(packet[i]) << " ";
    }
    std::cout << std::dec << std::endl;
}

bool DnsPacketBuilder::validate_domain_name(const std::string& domain) {
    if (domain.empty() || domain.size() > 253) {
        return false; // DNS domain max 253 karakter
    }

    auto labels = split_domain(domain);
    for (const auto& label : labels) {
        if (label.empty() || label.size() > 63) {
            return false; // DNS label max 63 karakter
        }

        // Alapvető karakter ellenőrzés
        for (char c : label) {
            if (!std::isalnum(c) && c != '-' && c != '_') {
                return false; // csak alfanumerikus, kötőjel, aláhúzás
            }
        }
    }

    return true;
}

} // namespace chimera
