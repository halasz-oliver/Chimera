#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "tl/expected.hpp"

namespace chimera {

enum class TransportError {
    SocketCreationFailed,
    SendFailed,
    ReceiveFailed,
    InvalidAddress,
    Timeout
};

class ITransport {
public:
    virtual ~ITransport() = default;
    virtual tl::expected<size_t, TransportError> send(const std::vector<uint8_t>& data) = 0;
    virtual tl::expected<std::vector<uint8_t>, TransportError> receive() = 0;
    virtual void set_timeout(std::chrono::milliseconds timeout) = 0;
};

// UDP transport implementáció
class TransportUdp : public ITransport {
    int sock_ = -1;
    sockaddr_in server_addr_{};
    std::chrono::milliseconds timeout_ = std::chrono::milliseconds(5000);

public:
    TransportUdp(const std::string& server_ip, uint16_t port) {
        sock_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock_ < 0) {
            std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
            return;
        }
        server_addr_.sin_family = AF_INET;
        server_addr_.sin_port = htons(port);
        if (inet_pton(AF_INET, server_ip.c_str(), &server_addr_.sin_addr) <= 0) {
            std::cerr << "Invalid server IP address" << std::endl;
            close(sock_);
            sock_ = -1;
            return;
        }
        set_timeout(timeout_);
    }

    ~TransportUdp() {
        if (sock_ >= 0) {
            close(sock_);
        }
    }

    tl::expected<size_t, TransportError> send(const std::vector<uint8_t>& data) override {
        if (sock_ < 0) return tl::unexpected(TransportError::SocketCreationFailed);
        ssize_t sent = sendto(sock_, data.data(), data.size(), 0,
                              reinterpret_cast<sockaddr*>(&server_addr_), sizeof(server_addr_));
        if (sent < 0) {
            return tl::unexpected(TransportError::SendFailed);
        }
        return static_cast<size_t>(sent);
    }

    tl::expected<std::vector<uint8_t>, TransportError> receive() override {
        if (sock_ < 0) return tl::unexpected(TransportError::SocketCreationFailed);
        std::vector<uint8_t> buffer(512);
        ssize_t received = recvfrom(sock_, buffer.data(), buffer.size(), 0, nullptr, nullptr);
        if (received < 0) {
            return tl::unexpected(TransportError::ReceiveFailed);
        }
        buffer.resize(received);
        return buffer;
    }

    void set_timeout(std::chrono::milliseconds timeout) override {
        timeout_ = timeout;
        if (sock_ >= 0) {
            struct timeval tv{};
            tv.tv_sec = timeout.count() / 1000;
            tv.tv_usec = (timeout.count() % 1000) * 1000;
            setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        }
    }
};

} // namespace chimera