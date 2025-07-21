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

// UDP transport implementation
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

// DoH (DNS-over-HTTPS) transport implementation
class TransportDoH : public ITransport {
    std::string server_url_;
    std::chrono::milliseconds timeout_ = std::chrono::milliseconds(5000);
    std::vector<uint8_t> last_response_; // Store response from HTTPS request
    
public:
    TransportDoH(const std::string& server_url) : server_url_(server_url) {
        // Ensure URL has proper format
        if (server_url_.find("https://") != 0) {
            server_url_ = "https://" + server_url_;
        }
        if (server_url_.back() != '/') {
            server_url_ += "/";
        }
        server_url_ += "dns-query";
    }

    tl::expected<size_t, TransportError> send(const std::vector<uint8_t>& data) override;
    tl::expected<std::vector<uint8_t>, TransportError> receive() override;
    void set_timeout(std::chrono::milliseconds timeout) override {
        timeout_ = timeout;
    }

private:
    tl::expected<std::vector<uint8_t>, TransportError> perform_https_request(const std::vector<uint8_t>& dns_query);
    std::string base64_url_encode(const std::vector<uint8_t>& data);
};

// DoT (DNS-over-TLS) transport implementation  
class TransportDoT : public ITransport {
    std::string server_ip_;
    uint16_t port_;
    std::chrono::milliseconds timeout_ = std::chrono::milliseconds(5000);
    void* ssl_ctx_ = nullptr;
    void* ssl_ = nullptr;
    int sock_ = -1;

public:
    TransportDoT(const std::string& server_ip, uint16_t port = 853) 
        : server_ip_(server_ip), port_(port) {}
    
    ~TransportDoT();

    tl::expected<size_t, TransportError> send(const std::vector<uint8_t>& data) override;
    tl::expected<std::vector<uint8_t>, TransportError> receive() override;
    void set_timeout(std::chrono::milliseconds timeout) override {
        timeout_ = timeout;
    }

private:
    tl::expected<void, TransportError> establish_tls_connection();
    void cleanup_connection();
};

} // namespace chimera