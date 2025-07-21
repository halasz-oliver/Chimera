#include "chimera/dns_packet.hpp"

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cctype>

namespace chimera {

std::random_device DnsPacketBuilder::rd;
std::mt19937 DnsPacketBuilder::gen(DnsPacketBuilder::rd());

std::vector<uint8_t> DnsPacketBuilder::build_query(const DnsQuestion& q, const std::string& payload) {
    std::vector<uint8_t> packet;
    try {
        DnsHeader hdr{};
        hdr.id = gen() & 0xFFFF;
        hdr.flags = 0x0100; // standard query with recursion desired
        hdr.qdcount = 1;
        hdr.ancount = 0;
        hdr.nscount = 0;
        hdr.arcount = 0;

        write_header(packet, hdr);
        write_question(packet, q);

        if (!payload.empty() && q.type == DnsType::TXT) {
            write_txt_data(packet, payload);
        }

        std::cout << "DNS packet created: " << packet.size() << " bytes, ID="
                  << std::hex << hdr.id << std::dec << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "DNS packet building error: " << e.what() << std::endl;
        throw;
    }
    return packet;
}

std::vector<uint8_t> DnsPacketBuilder::parse_response(const std::vector<uint8_t>& response, std::vector<DnsResourceRecord>& answers) {
    if (response.size() < 12) {
        throw std::runtime_error("DNS response too short");
    }

    size_t offset = 0;
    DnsHeader hdr{};
    hdr.id = (response[offset] << 8) | response[offset + 1]; offset += 2;
    hdr.flags = (response[offset] << 8) | response[offset + 1]; offset += 2;
    hdr.qdcount = (response[offset] << 8) | response[offset + 1]; offset += 2;
    hdr.ancount = (response[offset] << 8) | response[offset + 1]; offset += 2;
    hdr.nscount = (response[offset] << 8) | response[offset + 1]; offset += 2;
    hdr.arcount = (response[offset] << 8) | response[offset + 1]; offset += 2;

    // Skip questions
    for (int i = 0; i < hdr.qdcount; ++i) {
        std::string qname;
        offset += read_domain_name(response, offset, qname);
        offset += 4; // type(2) + class(2)
    }

    // Process answers
    for (int i = 0; i < hdr.ancount; ++i) {
        DnsResourceRecord rr;
        const size_t name_len = read_domain_name(response, offset, rr.name);
        offset += name_len;
        rr.type = static_cast<DnsType>(read_uint16(response, offset)); offset += 2;
        rr.cls = static_cast<DnsClass>(read_uint16(response, offset)); offset += 2;
        rr.ttl = read_uint32(response, offset); offset += 4;
        const uint16_t rdlength = read_uint16(response, offset); offset += 2;

        if (offset + rdlength > response.size()) {
            throw std::runtime_error("RDATA length exceeds response size");
        }

        rr.rdata.assign(response.begin() + offset, response.begin() + offset + rdlength);
        offset += rdlength;

        answers.push_back(std::move(rr));
    }

    return {};
}

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
        return false;
    }
    auto labels = split_domain(domain);
    for (const auto& label : labels) {
        if (label.empty() || label.size() > 63) {
            return false;
        }
        for (char c : label) {
            if (!std::isalnum(c) && c != '-' && c != '_') {
                return false;
            }
        }
    }
    return true;
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
        packet.push_back(0);
        return;
    }
    auto labels = split_domain(name);
    for (const auto& label : labels) {
        if (label.empty()) continue;
        if (label.size() > 63) {
            throw std::runtime_error("DNS label too long: " + label);
        }
        packet.push_back(static_cast<uint8_t>(label.size()));
        packet.insert(packet.end(), label.begin(), label.end());
    }
    packet.push_back(0);
}

void DnsPacketBuilder::write_txt_data(std::vector<uint8_t>& packet, const std::string& data) {
    if (data.size() > 255) {
        throw std::runtime_error("TXT data too long: " + std::to_string(data.size()));
    }
    packet.push_back(static_cast<uint8_t>(data.size()));
    packet.insert(packet.end(), data.begin(), data.end());
}

void DnsPacketBuilder::write_uint16(std::vector<uint8_t>& packet, uint16_t value) {
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
    if (!current.empty()) {
        labels.push_back(current);
    }
    return labels;
}

size_t DnsPacketBuilder::read_domain_name(const std::vector<uint8_t>& data, size_t offset, std::string& out_name) {
    size_t original_offset = offset;
    out_name.clear();
    bool jumped = false;
    size_t jump_offset = 0;
    size_t max_jumps = 5;
    size_t jumps = 0;

    while (true) {
        if (offset >= data.size()) {
            throw std::runtime_error("DNS name reading exceeds bounds");
        }
        uint8_t len = data[offset];
        if (len == 0) {
            offset++;
            break;
        }
        // Pointer check (2 MSB set)
        if ((len & 0xC0) == 0xC0) {
            if (offset + 1 >= data.size()) {
                throw std::runtime_error("DNS pointer exceeds bounds");
            }
            uint16_t pointer = ((len & 0x3F) << 8) | data[offset + 1];
            if (!jumped) {
                jump_offset = offset + 2;
            }
            offset = pointer;
            jumped = true;
            jumps++;
            if (jumps > max_jumps) {
                throw std::runtime_error("Too many DNS pointer jumps");
            }
            continue;
        }
        offset++;
        if (offset + len > data.size()) {
            throw std::runtime_error("DNS label length exceeds bounds");
        }
        if (!out_name.empty()) {
            out_name += ".";
        }
        out_name.append(reinterpret_cast<const char*>(&data[offset]), len);
        offset += len;
    }
    if (!jumped) {
        return offset - original_offset;
    } else {
        return jump_offset - original_offset;
    }
}

uint16_t DnsPacketBuilder::read_uint16(const std::vector<uint8_t>& data, size_t offset) {
    if (offset + 1 >= data.size()) {
        throw std::runtime_error("read_uint16 exceeds bounds");
    }
    return (data[offset] << 8) | data[offset + 1];
}

uint32_t DnsPacketBuilder::read_uint32(const std::vector<uint8_t>& data, size_t offset) {
    if (offset + 3 >= data.size()) {
        throw std::runtime_error("read_uint32 exceeds bounds");
    }
    return (data[offset] << 24) | (data[offset + 1] << 16) | (data[offset + 2] << 8) | data[offset + 3];
}

} // namespace chimera
