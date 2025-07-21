#include "chimera/Transport.hpp"
#include <curl/curl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sstream>
#include <algorithm>
#include <fcntl.h>

namespace chimera {

// Callback function for libcurl to write data
struct CurlResponse {
    std::vector<uint8_t> data;
};

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, CurlResponse* response) {
    size_t totalSize = size * nmemb;
    uint8_t* bytes = static_cast<uint8_t*>(contents);
    response->data.insert(response->data.end(), bytes, bytes + totalSize);
    return totalSize;
}

// DoH Implementation
tl::expected<size_t, TransportError> TransportDoH::send(const std::vector<uint8_t>& data) {
    auto response = perform_https_request(data);
    if (!response) {
        return tl::unexpected(response.error());
    }
    
    // Store the response for later retrieval
    last_response_ = std::move(response.value());
    return data.size(); // Return the sent data size
}

tl::expected<std::vector<uint8_t>, TransportError> TransportDoH::receive() {
    // Return the stored response from the last send operation
    if (last_response_.empty()) {
        return tl::unexpected(TransportError::ReceiveFailed);
    }
    
    auto response = std::move(last_response_);
    last_response_.clear();
    return response;
}

tl::expected<std::vector<uint8_t>, TransportError> TransportDoH::perform_https_request(const std::vector<uint8_t>& dns_query) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return tl::unexpected(TransportError::SendFailed);
    }

    CurlResponse response;
    struct curl_slist* headers = nullptr;

    // Encode DNS query as base64url for GET parameter or POST body
    std::string encoded_query = base64_url_encode(dns_query);
    std::string full_url = server_url_ + "?dns=" + encoded_query;

    // Set headers for DNS-over-HTTPS
    headers = curl_slist_append(headers, "Accept: application/dns-message");
    headers = curl_slist_append(headers, "Content-Type: application/dns-message");

    curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout_.count());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode res = curl_easy_perform(curl);
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        return tl::unexpected(TransportError::SendFailed);
    }

    return response.data;
}

std::string TransportDoH::base64_url_encode(const std::vector<uint8_t>& data) {
    // Base64 URL-safe encoding (RFC 4648 Section 5)
    const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    std::string result;
    
    for (size_t i = 0; i < data.size(); i += 3) {
        uint32_t val = data[i] << 16;
        if (i + 1 < data.size()) val |= data[i + 1] << 8;
        if (i + 2 < data.size()) val |= data[i + 2];

        result += chars[(val >> 18) & 63];
        result += chars[(val >> 12) & 63];
        result += (i + 1 < data.size()) ? chars[(val >> 6) & 63] : '=';
        result += (i + 2 < data.size()) ? chars[val & 63] : '=';
    }
    
    // Remove padding for URL-safe encoding
    while (!result.empty() && result.back() == '=') {
        result.pop_back();
    }
    
    return result;
}

// DoT Implementation
TransportDoT::~TransportDoT() {
    cleanup_connection();
}

tl::expected<size_t, TransportError> TransportDoT::send(const std::vector<uint8_t>& data) {
    if (!ssl_) {
        auto conn_result = establish_tls_connection();
        if (!conn_result) {
            return tl::unexpected(conn_result.error());
        }
    }

    // DNS-over-TLS uses a 2-byte length prefix
    std::vector<uint8_t> prefixed_data;
    uint16_t length = htons(static_cast<uint16_t>(data.size()));
    prefixed_data.insert(prefixed_data.end(), 
                        reinterpret_cast<uint8_t*>(&length), 
                        reinterpret_cast<uint8_t*>(&length) + 2);
    prefixed_data.insert(prefixed_data.end(), data.begin(), data.end());

    int sent = SSL_write(static_cast<SSL*>(ssl_), prefixed_data.data(), prefixed_data.size());
    if (sent <= 0) {
        cleanup_connection();
        return tl::unexpected(TransportError::SendFailed);
    }

    return static_cast<size_t>(sent - 2); // Return size without length prefix
}

tl::expected<std::vector<uint8_t>, TransportError> TransportDoT::receive() {
    if (!ssl_) {
        return tl::unexpected(TransportError::ReceiveFailed);
    }

    // Read length prefix first
    uint16_t length;
    int read_len = SSL_read(static_cast<SSL*>(ssl_), &length, 2);
    if (read_len != 2) {
        cleanup_connection();
        return tl::unexpected(TransportError::ReceiveFailed);
    }

    length = ntohs(length);
    std::vector<uint8_t> buffer(length);

    int total_read = 0;
    while (total_read < length) {
        int read_bytes = SSL_read(static_cast<SSL*>(ssl_), 
                                 buffer.data() + total_read, 
                                 length - total_read);
        if (read_bytes <= 0) {
            cleanup_connection();
            return tl::unexpected(TransportError::ReceiveFailed);
        }
        total_read += read_bytes;
    }

    return buffer;
}

tl::expected<void, TransportError> TransportDoT::establish_tls_connection() {
    // Initialize OpenSSL
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    const SSL_METHOD* method = TLS_client_method();
    SSL_CTX* ctx = SSL_CTX_new(method);
    if (!ctx) {
        return tl::unexpected(TransportError::SocketCreationFailed);
    }

    ssl_ctx_ = ctx;

    // Create socket
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ < 0) {
        return tl::unexpected(TransportError::SocketCreationFailed);
    }

    // Set up server address
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    if (inet_pton(AF_INET, server_ip_.c_str(), &server_addr.sin_addr) <= 0) {
        close(sock_);
        sock_ = -1;
        return tl::unexpected(TransportError::InvalidAddress);
    }

    // Set socket timeout
    struct timeval tv{};
    tv.tv_sec = timeout_.count() / 1000;
    tv.tv_usec = (timeout_.count() % 1000) * 1000;
    setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock_, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    // Connect
    if (connect(sock_, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        close(sock_);
        sock_ = -1;
        return tl::unexpected(TransportError::Timeout);
    }

    // Create SSL connection
    SSL* ssl = SSL_new(static_cast<SSL_CTX*>(ssl_ctx_));
    if (!ssl) {
        close(sock_);
        sock_ = -1;
        return tl::unexpected(TransportError::SocketCreationFailed);
    }

    ssl_ = ssl;
    SSL_set_fd(static_cast<SSL*>(ssl_), sock_);

    // Perform TLS handshake
    if (SSL_connect(static_cast<SSL*>(ssl_)) != 1) {
        cleanup_connection();
        return tl::unexpected(TransportError::Timeout);
    }

    return {};
}

void TransportDoT::cleanup_connection() {
    if (ssl_) {
        SSL_shutdown(static_cast<SSL*>(ssl_));
        SSL_free(static_cast<SSL*>(ssl_));
        ssl_ = nullptr;
    }
    
    if (ssl_ctx_) {
        SSL_CTX_free(static_cast<SSL_CTX*>(ssl_ctx_));
        ssl_ctx_ = nullptr;
    }
    
    if (sock_ >= 0) {
        close(sock_);
        sock_ = -1;
    }
}

} // namespace chimera