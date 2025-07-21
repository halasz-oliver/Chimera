#pragma once

#include <array>
#include <string>
#include <stdexcept>

// Base64 encoder/decoder - now with full padding support
namespace chimera {

class Base64 {
    static constexpr std::array<char, 64> tbl = {
        'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
        'Q','R','S','T','U','V','W','X','Y','Z',
        'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
        'q','r','s','t','u','v','w','x','y','z',
        '0','1','2','3','4','5','6','7','8','9','+','/'
    };

public:
    static std::string encode(const std::string& in) {
        if (in.empty()) return "";

        std::string out;
        out.reserve(((in.size() + 2) / 3) * 4);

        for (size_t i = 0; i < in.size(); i += 3) {
            uint32_t b = static_cast<uint8_t>(in[i]) << 16;
            if (i+1 < in.size()) b |= static_cast<uint8_t>(in[i + 1]) << 8;
            if (i+2 < in.size()) b |= static_cast<uint8_t>(in[i + 2]);

            out += tbl[(b>>18)&0x3F];
            out += tbl[(b>>12)&0x3F];
            out += (i+1 < in.size()) ? tbl[(b>>6)&0x3F] : '=';
            out += (i+2 < in.size()) ? tbl[b&0x3F] : '=';
        }

        return out;
    }

    static std::string decode(const std::string& in) {
        if (in.empty()) return "";
        if (in.size() % 4 != 0) {
            throw std::runtime_error("Invalid base64 length");
        }

        auto char_to_val = [](char c) -> int {
            if (c>='A'&&c<='Z') return c-'A';
            if (c>='a'&&c<='z') return c-'a'+26;
            if (c>='0'&&c<='9') return c-'0'+52;
            if (c=='+') return 62;
            if (c=='/') return 63;
            if (c=='=') return -1; // padding
            throw std::runtime_error("Invalid base64 character");
        };

        std::string out;
        out.reserve((in.size() * 3) / 4);

        for (size_t i = 0; i < in.size(); i += 4) {
            int vals[4];
            for (int j = 0; j < 4; j++) {
                vals[j] = char_to_val(in[i+j]);
            }

            // Padding calculation
            int pad = 0;
            if (vals[3] == -1) pad++;
            if (vals[2] == -1) pad++;

            uint32_t b = (vals[0] << 18) | (vals[1] << 12);
            if (pad < 2) b |= (vals[2] << 6);
            if (pad < 1) b |= vals[3];

            out += static_cast<char>((b >> 16) & 0xFF);
            if (pad < 2) out += static_cast<char>((b >> 8) & 0xFF);
            if (pad < 1) out += static_cast<char>(b & 0xFF);
        }

        return out;
    }
};

} // namespace chimera
