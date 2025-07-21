#include "chimera/AsyncIO.hpp"
#include "chimera/client.hpp"
#include "chimera/Transport.hpp"
#include "chimera/base64.hpp"
#include "chimera/dns_packet.hpp"
#include "chimera/BehavioralMimicry.hpp"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>

#ifdef __APPLE__
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>
#elif __linux__
#include <sys/epoll.h>
#elif _WIN32
#include <winsock2.h>
#endif

namespace chimera {

class AsyncIOManager::Impl {
    std::queue<std::unique_ptr<AsyncRequest>> pending_requests_;
    std::queue<std::unique_ptr<AsyncRequest>> active_requests_;
    std::mutex requests_mutex_;
    std::condition_variable requests_cv_;
    std::atomic<bool> running_{false};
    std::thread worker_thread_;
    
#ifdef __APPLE__
    int kqueue_fd_ = -1;
#elif __linux__
    int epoll_fd_ = -1;
#endif

public:
    Impl() {
#ifdef __APPLE__
        kqueue_fd_ = kqueue();
        if (kqueue_fd_ == -1) {
            std::cerr << "Failed to create kqueue" << std::endl;
        }
#elif __linux__
        epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
        if (epoll_fd_ == -1) {
            std::cerr << "Failed to create epoll" << std::endl;
        }
#endif
    }
    
    ~Impl() {
        stop_background_processing();
#ifdef __APPLE__
        if (kqueue_fd_ >= 0) {
            close(kqueue_fd_);
        }
#elif __linux__
        if (epoll_fd_ >= 0) {
            close(epoll_fd_);
        }
#endif
    }
    
    void submit_request(std::unique_ptr<AsyncRequest> request) {
        std::lock_guard<std::mutex> lock(requests_mutex_);
        request->start_time = std::chrono::steady_clock::now();
        pending_requests_.push(std::move(request));
        requests_cv_.notify_one();
    }
    
    void start_background_processing() {
        if (running_.exchange(true)) {
            return; // Already running
        }
        
        worker_thread_ = std::thread([this]() {
            while (running_) {
                process_events_internal(std::chrono::milliseconds(100));
            }
        });
    }
    
    void stop_background_processing() {
        if (!running_.exchange(false)) {
            return; // Already stopped
        }
        
        requests_cv_.notify_all();
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
    }
    
    size_t pending_requests() const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(requests_mutex_));
        return pending_requests_.size() + active_requests_.size();
    }
    
    void process_events_internal(std::chrono::milliseconds timeout) {
        // Move pending requests to active
        {
            std::lock_guard<std::mutex> lock(requests_mutex_);
            while (!pending_requests_.empty()) {
                active_requests_.push(std::move(pending_requests_.front()));
                pending_requests_.pop();
            }
        }
        
        // Process active requests
        std::queue<std::unique_ptr<AsyncRequest>> completed_requests;
        
        while (!active_requests_.empty()) {
            auto request = std::move(active_requests_.front());
            active_requests_.pop();
            
            // Check timeout
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - request->start_time);
            
            if (elapsed >= request->timeout) {
                // Timeout
                AsyncResult result{
                    .success = false,
                    .data = {},
                    .latency = elapsed,
                    .error = TransportError::Timeout
                };
                request->callback(result);
                continue;
            }
            
            // Process the request asynchronously
            std::thread([req = std::move(request)]() mutable {
                process_single_request(std::move(req));
            }).detach();
        }
        
        // Wait for a short time to avoid busy waiting
        if (running_) {
            std::unique_lock<std::mutex> lock(requests_mutex_);
            requests_cv_.wait_for(lock, timeout, [this]() {
                return !pending_requests_.empty() || !running_;
            });
        }
    }
    
    static void process_single_request(std::unique_ptr<AsyncRequest> request) {
        auto start_time = request->start_time;
        
        try {
            // Send the request
            auto send_result = request->transport->send(request->dns_query);
            if (!send_result) {
                AsyncResult result{
                    .success = false,
                    .data = {},
                    .latency = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - start_time),
                    .error = send_result.error()
                };
                request->callback(result);
                return;
            }
            
            // Receive the response
            auto recv_result = request->transport->receive();
            auto end_time = std::chrono::steady_clock::now();
            auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            
            if (!recv_result) {
                AsyncResult result{
                    .success = false,
                    .data = {},
                    .latency = latency,
                    .error = recv_result.error()
                };
                request->callback(result);
                return;
            }
            
            // Success
            AsyncResult result{
                .success = true,
                .data = recv_result.value(),
                .latency = latency,
                .error = TransportError::SocketCreationFailed  // Unused for success
            };
            request->callback(result);
            
        } catch (const std::exception& e) {
            AsyncResult result{
                .success = false,
                .data = {},
                .latency = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - start_time),
                .error = TransportError::SendFailed
            };
            request->callback(result);
        }
    }
};

// AsyncIOManager implementation
AsyncIOManager::AsyncIOManager() : impl_(std::make_unique<Impl>()) {}
AsyncIOManager::~AsyncIOManager() = default;

void AsyncIOManager::submit_request(std::unique_ptr<AsyncRequest> request) {
    impl_->submit_request(std::move(request));
}

void AsyncIOManager::process_events(std::chrono::milliseconds timeout) {
    impl_->process_events_internal(timeout);
}

void AsyncIOManager::start_background_processing() {
    impl_->start_background_processing();
}

void AsyncIOManager::stop_background_processing() {
    impl_->stop_background_processing();
}

size_t AsyncIOManager::pending_requests() const {
    return impl_->pending_requests();
}

// AsyncChimeraClient implementation
AsyncChimeraClient::AsyncChimeraClient(ClientConfig config) : config_(std::move(config)) {}

void AsyncChimeraClient::send_text_async(const std::string& message, AsyncCallback callback) {
    // Apply behavioral mimicry
    if (config_.adaptive_transport) {
        BehavioralMimicry mimicry(config_.behavioral_profile);
        mimicry.apply_behavioral_delay();
    }
    
    // Create DNS query
    std::string encoded_message;
    try {
        encoded_message = Base64::encode(message);
    } catch (const std::exception& e) {
        AsyncResult result{
            .success = false,
            .data = {},
            .latency = std::chrono::milliseconds(0),
            .error = TransportError::SendFailed
        };
        callback(result);
        return;
    }
    
    // Generate domain
    std::string target_domain = config_.target_domain;
    if (config_.use_random_subdomains) {
        // Simple random subdomain generation
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1000, 9999);
        target_domain = "rnd" + std::to_string(dis(gen)) + "." + config_.target_domain;
    }
    
    // Build DNS packet
    DnsQuestion question{target_domain, DnsType::TXT};
    std::vector<uint8_t> packet;
    try {
        packet = DnsPacketBuilder::build_query(question, encoded_message);
    } catch (const std::exception& e) {
        AsyncResult result{
            .success = false,
            .data = {},
            .latency = std::chrono::milliseconds(0),
            .error = TransportError::SendFailed
        };
        callback(result);
        return;
    }
    
    // Create transport
    std::unique_ptr<ITransport> transport;
    if (config_.transport == TransportType::UDP) {
        transport = std::make_unique<TransportUdp>(config_.dns_server, config_.dns_port);
    } else if (config_.transport == TransportType::DoH) {
        transport = std::make_unique<TransportDoH>(config_.dns_server);
    } else if (config_.transport == TransportType::DoT) {
        transport = std::make_unique<TransportDoT>(config_.dns_server, config_.dns_port);
    }
    
    if (!transport) {
        AsyncResult result{
            .success = false,
            .data = {},
            .latency = std::chrono::milliseconds(0),
            .error = TransportError::SocketCreationFailed
        };
        callback(result);
        return;
    }
    
    transport->set_timeout(config_.timeout);
    
    // Create async request
    auto request = std::make_unique<AsyncRequest>();
    request->dns_query = std::move(packet);
    request->transport = std::move(transport);
    request->callback = callback;
    request->timeout = config_.timeout;
    
    io_manager_.submit_request(std::move(request));
}

std::future<AsyncResult> AsyncChimeraClient::send_text_future(const std::string& message) {
    auto promise = std::make_shared<std::promise<AsyncResult>>();
    auto future = promise->get_future();
    
    send_text_async(message, [promise](const AsyncResult& result) {
        promise->set_value(result);
    });
    
    return future;
}

void AsyncChimeraClient::ping_async(AsyncCallback callback) {
    DnsQuestion ping_question{"ping.test", DnsType::A};
    std::vector<uint8_t> packet;
    try {
        packet = DnsPacketBuilder::build_query(ping_question);
    } catch (const std::exception& e) {
        AsyncResult result{
            .success = false,
            .data = {},
            .latency = std::chrono::milliseconds(0),
            .error = TransportError::SendFailed
        };
        callback(result);
        return;
    }
    
    // Create transport
    std::unique_ptr<ITransport> transport;
    if (config_.transport == TransportType::UDP) {
        transport = std::make_unique<TransportUdp>(config_.dns_server, config_.dns_port);
    } else if (config_.transport == TransportType::DoH) {
        transport = std::make_unique<TransportDoH>(config_.dns_server);
    } else if (config_.transport == TransportType::DoT) {
        transport = std::make_unique<TransportDoT>(config_.dns_server, config_.dns_port);
    }
    
    if (!transport) {
        AsyncResult result{
            .success = false,
            .data = {},
            .latency = std::chrono::milliseconds(0),
            .error = TransportError::SocketCreationFailed
        };
        callback(result);
        return;
    }
    
    transport->set_timeout(config_.timeout);
    
    auto request = std::make_unique<AsyncRequest>();
    request->dns_query = std::move(packet);
    request->transport = std::move(transport);
    request->callback = callback;
    request->timeout = config_.timeout;
    
    io_manager_.submit_request(std::move(request));
}

std::future<AsyncResult> AsyncChimeraClient::ping_future() {
    auto promise = std::make_shared<std::promise<AsyncResult>>();
    auto future = promise->get_future();
    
    ping_async([promise](const AsyncResult& result) {
        promise->set_value(result);
    });
    
    return future;
}

} // namespace chimera