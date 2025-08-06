#include "chimera/config_wizard.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <regex>
#include <filesystem>
#include <limits>

namespace chimera {

ConfigWizard::ConfigWizard() {
    initializeValidationRules();
}

void ConfigWizard::initializeValidationRules() {
    // Comprehensive DNS Server validation (supports IPv4 and IPv6)
    validationRules["dns_server"] = [](const std::string& value) {
        std::regex ipv4_regex(R"(^(\d{1,3}\.){3}\d{1,3}$)");
        std::regex ipv6_regex(R"(^([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}$)");
        std::regex hostname_regex(R"(^[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
        return std::regex_match(value, ipv4_regex) || 
               std::regex_match(value, ipv6_regex) || 
               std::regex_match(value, hostname_regex);
    };

    // Enhanced domain validation
    validationRules["target_domain"] = [](const std::string& value) {
        std::regex domain_regex(R"(^[a-zA-Z0-9][a-zA-Z0-9-]{1,61}[a-zA-Z0-9](\.[a-zA-Z]{2,})+$)");
        return std::regex_match(value, domain_regex);
    };
}

bool ConfigWizard::validateInput(const std::string& field, const std::string& value) {
    if (validationRules.find(field) != validationRules.end()) {
        return validationRules[field](value);
    }
    return true;
}

void ConfigWizard::displayHeader() {
    std::cout << "\nChimera Configuration Wizard v1.1\n";
    std::cout << "====================================\n\n";
    std::cout << "Interactive configuration for steganographic communication\n";
    std::cout << "Secure, private, resilient communication framework\n\n";
}

// Helper function for input validation and error handling
template<typename T>
T getValidInput(const std::string& prompt, 
                std::function<bool(const T&)> validator = [](const T&){ return true; }) {
    T input;
    while (true) {
        std::cout << prompt;
        if (std::cin >> input) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (validator(input)) return input;
        } else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cout << "Invalid input. Please try again.\n";
    }
}

ClientConfig ConfigWizard::runInteractiveSetup() {
    ClientConfig config;
    displayHeader();

    // DNS Server Configuration with multiple validation checks
    config.dns_server = [this]() {
        while (true) {
            std::string dns_server;
            std::cout << "Enter DNS Server (IPv4/IPv6/Hostname, e.g., 8.8.8.8): ";
            std::getline(std::cin, dns_server);

            if (validateInput("dns_server", dns_server)) {
                return dns_server;
            }
            std::cout << "Invalid DNS server. Use IPv4, IPv6, or valid hostname.\n";
        }
    }();

    // Target Domain Configuration
    config.target_domain = [this]() {
        while (true) {
            std::string target_domain;
            std::cout << "Enter Target Domain (e.g., example.com): ";
            std::getline(std::cin, target_domain);

            if (validateInput("target_domain", target_domain)) {
                return target_domain;
            }
            std::cout << "Invalid domain. Use valid domain format.\n";
        }
    }();

    // Transport Layer Selection with more descriptive options
    std::vector<std::pair<TransportType, std::string>> transport_options = {
        {TransportType::DoH, "DNS over HTTPS (Recommended, Most Secure)"},
        {TransportType::DoT, "DNS over TLS (High Security)"},
        {TransportType::UDP, "Standard UDP (Legacy, Less Secure)"}
    };

    std::cout << "\nSelect transport layer:\n";
    for (size_t i = 0; i < transport_options.size(); ++i) {
        std::cout << i+1 << ". " << transport_options[i].second << std::endl;
    }

    config.transport = [&transport_options]() {
        while (true) {
            int choice = getValidInput<int>(
                "Enter transport layer number: ", 
                [&transport_options](int val) { 
                    return val > 0 && val <= static_cast<int>(transport_options.size()); 
                }
            );
            return transport_options[choice - 1].first;
        }
    }();

    // Encoding Strategy with Risk Assessment
    std::vector<std::pair<EncodingStrategy, std::string>> encoding_options = {
        {EncodingStrategy::SINGLE_RECORD, "Single Record (Lower Capacity, More Subtle)"},
        {EncodingStrategy::MULTI_RECORD, "Multi-Record (Higher Capacity, More Complex)"}
    };

    std::cout << "\nSelect encoding strategy:\n";
    for (size_t i = 0; i < encoding_options.size(); ++i) {
        std::cout << i+1 << ". " << encoding_options[i].second << std::endl;
    }

    config.encoding_strategy = [&encoding_options]() {
        while (true) {
            int choice = getValidInput<int>(
                "Enter encoding strategy number: ", 
                [&encoding_options](int val) { 
                    return val > 0 && val <= static_cast<int>(encoding_options.size()); 
                }
            );
            return encoding_options[choice - 1].first;
        }
    }();

    // Advanced Settings with Risk Warning
    std::cout << "\nAdvanced settings (configure with caution)\n";
    
    config.use_compression = getValidInput<char>(
        "Enable Compression? (y/n): ", 
        [](char c) { 
            return std::tolower(c) == 'y' || std::tolower(c) == 'n'; 
        }
    ) == 'y';

    config.noise_ratio = getValidInput<double>(
        "Enter Noise Ratio (0.0 - 1.0, default 0.1): ", 
        [](double val) { 
            return val >= 0.0 && val <= 1.0; 
        }
    );

    // Security Warning
    std::cout << "\nSecurity warning:\n";
    std::cout << "Configuring advanced settings may impact communication patterns.\n";
    std::cout << "Recommended: Use default settings for optimal stealth.\n\n";

    displayConfigSummary(config);
    return config;
}

void ConfigWizard::displayConfigSummary(const ClientConfig& config) {
    std::cout << "\nConfiguration summary\n";
    std::cout << "======================\n";
    std::cout << "DNS Server:         " << config.dns_server << std::endl;
    std::cout << "Target Domain:      " << config.target_domain << std::endl;
    std::cout << "Transport Layer:    " << transportToString(config.transport) << std::endl;
    std::cout << "Encoding Strategy:  " << encodingToString(config.encoding_strategy) << std::endl;
    std::cout << "Compression:        " << (config.use_compression ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Noise Ratio:        " << std::fixed << std::setprecision(2) 
              << config.noise_ratio << std::endl;
    
    // Risk Assessment
    std::cout << "\nRisk assessment:\n";
    std::cout << (config.transport == TransportType::DoH ? "High security" : "Moderate security") << std::endl;
    std::cout << (config.encoding_strategy == EncodingStrategy::MULTI_RECORD ? 
                  "Advanced steganographic encoding" : 
                  "Basic steganographic encoding") << std::endl;
}

// Existing toString methods remain the same
std::string ConfigWizard::transportToString(TransportType type) {
    switch (type) {
        case TransportType::DoH: return "DNS over HTTPS";
        case TransportType::DoT: return "DNS over TLS";
        case TransportType::UDP: return "UDP";
    }
    return "Unknown";
}

std::string ConfigWizard::encodingToString(EncodingStrategy strategy) {
    switch (strategy) {
        case EncodingStrategy::SINGLE_RECORD: return "Single record";
        case EncodingStrategy::MULTI_RECORD: return "Multi record";
    }
    return "Unknown";
}

}  // namespace chimera