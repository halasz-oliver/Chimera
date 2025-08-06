#include "chimera/config_wizard.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <regex>
#include <filesystem>

namespace chimera {

ConfigWizard::ConfigWizard() {
    initializeValidationRules();
}

void ConfigWizard::initializeValidationRules() {
    // DNS Server validation
    validationRules["dns_server"] = [](const std::string& value) {
        std::regex ip_regex(R"(^(\d{1,3}\.){3}\d{1,3}$)");
        return std::regex_match(value, ip_regex);
    };

    // Domain validation
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
    std::cout << "\n🌐 Chimera Configuration Wizard 🔐\n";
    std::cout << "==============================\n\n";
    std::cout << "Welcome to the interactive configuration setup for Chimera.\n";
    std::cout << "This wizard will help you configure your steganographic communication settings.\n\n";
}

ClientConfig ConfigWizard::runInteractiveSetup() {
    ClientConfig config;
    displayHeader();

    // DNS Server Configuration
    while (true) {
        std::cout << "Enter DNS Server IP (e.g., 8.8.8.8): ";
        std::string dns_server;
        std::getline(std::cin, dns_server);

        if (validateInput("dns_server", dns_server)) {
            config.dns_server = dns_server;
            break;
        } else {
            std::cout << "❌ Invalid DNS server IP. Please use a valid IPv4 address.\n";
        }
    }

    // Target Domain Configuration
    while (true) {
        std::cout << "Enter Target Domain (e.g., example.com): ";
        std::string target_domain;
        std::getline(std::cin, target_domain);

        if (validateInput("target_domain", target_domain)) {
            config.target_domain = target_domain;
            break;
        } else {
            std::cout << "❌ Invalid domain name. Please use a valid domain format.\n";
        }
    }

    // Transport Layer Selection
    std::vector<std::string> transport_options = {"DoH", "DoT", "UDP"};
    std::cout << "\nSelect Transport Layer:\n";
    for (size_t i = 0; i < transport_options.size(); ++i) {
        std::cout << i+1 << ". " << transport_options[i] << std::endl;
    }

    int transport_choice;
    while (true) {
        std::cout << "Enter transport layer number: ";
        std::cin >> transport_choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (transport_choice > 0 && transport_choice <= transport_options.size()) {
            config.transport = static_cast<TransportType>(transport_choice - 1);
            break;
        } else {
            std::cout << "❌ Invalid selection. Please choose a number between 1-" 
                      << transport_options.size() << std::endl;
        }
    }

    // Encoding Strategy
    std::vector<std::string> encoding_options = {"Single Record", "Multi-Record"};
    std::cout << "\nSelect Encoding Strategy:\n";
    for (size_t i = 0; i < encoding_options.size(); ++i) {
        std::cout << i+1 << ". " << encoding_options[i] << std::endl;
    }

    int encoding_choice;
    while (true) {
        std::cout << "Enter encoding strategy number: ";
        std::cin >> encoding_choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (encoding_choice > 0 && encoding_choice <= encoding_options.size()) {
            config.encoding_strategy = (encoding_choice == 1) 
                ? EncodingStrategy::SINGLE_RECORD 
                : EncodingStrategy::MULTI_RECORD;
            break;
        } else {
            std::cout << "❌ Invalid selection. Please choose a number between 1-" 
                      << encoding_options.size() << std::endl;
        }
    }

    // Optional Advanced Settings
    std::cout << "\n🔧 Advanced Settings (Optional)\n";
    std::cout << "Enable Compression? (y/n): ";
    char compress_choice;
    std::cin >> compress_choice;
    config.use_compression = (std::tolower(compress_choice) == 'y');

    std::cout << "Enter Noise Ratio (0.0 - 1.0, default 0.1): ";
    std::cin >> config.noise_ratio;
    config.noise_ratio = std::clamp(config.noise_ratio, 0.0, 1.0);

    displayConfigSummary(config);
    return config;
}

void ConfigWizard::displayConfigSummary(const ClientConfig& config) {
    std::cout << "\n📋 Configuration Summary\n";
    std::cout << "=======================\n";
    std::cout << "DNS Server:         " << config.dns_server << std::endl;
    std::cout << "Target Domain:      " << config.target_domain << std::endl;
    std::cout << "Transport Layer:    " << transportToString(config.transport) << std::endl;
    std::cout << "Encoding Strategy:  " << encodingToString(config.encoding_strategy) << std::endl;
    std::cout << "Compression:        " << (config.use_compression ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Noise Ratio:        " << std::fixed << std::setprecision(2) << config.noise_ratio << std::endl;
}

std::string ConfigWizard::transportToString(TransportType type) {
    switch(type) {
        case TransportType::DoH: return "DNS over HTTPS (DoH)";
        case TransportType::DoT: return "DNS over TLS (DoT)";
        case TransportType::UDP: return "UDP";
        default: return "Unknown";
    }
}

std::string ConfigWizard::encodingToString(EncodingStrategy strategy) {
    switch(strategy) {
        case EncodingStrategy::SINGLE_RECORD: return "Single Record";
        case EncodingStrategy::MULTI_RECORD: return "Multi-Record";
        default: return "Unknown";
    }
}

}  // namespace chimera