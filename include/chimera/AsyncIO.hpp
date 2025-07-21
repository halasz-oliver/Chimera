#pragma once

#include <memory>
#include <functional>
#include <vector>
#include <chrono>
#include <future>
#include "common.hpp"
#include "client.hpp"
#include "Transport.hpp"

namespace chimera {

// Async I/O result
struct AsyncResult {
    bool success;
    std::vector<uint8_t> data;
    std::chrono::milliseconds latency;
    TransportError error;
};

// Callback for async operations
using AsyncCallback = std::function<void(const AsyncResult&)>;

// Async DNS request
struct AsyncRequest {
    std::vector<uint8_t> dns_query;
    std::unique_ptr<ITransport> transport;
    AsyncCallback callback;
    std::chrono::steady_clock::time_point start_time;
    std::chrono::milliseconds timeout;
};

// High-performance async I/O manager
class AsyncIOManager {
public:
    AsyncIOManager();
    ~AsyncIOManager();

    // Submit async DNS request
    void submit_request(std::unique_ptr<AsyncRequest> request);
    
    // Process pending requests (call periodically)
    void process_events(std::chrono::milliseconds timeout = std::chrono::milliseconds(100));
    
    // Start background processing thread
    void start_background_processing();
    
    // Stop background processing
    void stop_background_processing();
    
    // Get number of pending requests
    size_t pending_requests() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// Async version of ChimeraClient
class AsyncChimeraClient {
    AsyncIOManager io_manager_;
    ClientConfig config_;
    
public:
    explicit AsyncChimeraClient(ClientConfig config);
    
    // Async send with callback
    void send_text_async(const std::string& message, AsyncCallback callback);
    
    // Async send with future
    std::future<AsyncResult> send_text_future(const std::string& message);
    
    // Async ping
    void ping_async(AsyncCallback callback);
    std::future<AsyncResult> ping_future();
    
    // Start/stop async processing
    void start() { io_manager_.start_background_processing(); }
    void stop() { io_manager_.stop_background_processing(); }
    
    // Configuration
    const ClientConfig& get_config() const { return config_; }
    void update_config(ClientConfig new_config) { config_ = std::move(new_config); }
};

} // namespace chimera