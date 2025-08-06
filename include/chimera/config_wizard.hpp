#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include "client.hpp"

namespace chimera {

class ConfigWizard {
public:
    ConfigWizard();
    
    // Run interactive configuration setup
    ClientConfig runInteractiveSetup();

private:
    // Validation rules for configuration fields
    std::unordered_map<std::string, std::function<bool(const std::string&)>> validationRules;

    // Initialize validation rules for configuration fields
    void initializeValidationRules();

    // Validate input for a specific configuration field
    bool validateInput(const std::string& field, const std::string& value);

    // Display wizard header
    void displayHeader();

    // Display final configuration summary
    void displayConfigSummary(const ClientConfig& config);

    // Convert transport type to human-readable string
    std::string transportToString(TransportType type);

    // Convert encoding strategy to human-readable string
    std::string encodingToString(EncodingStrategy strategy);
};

}  // namespace chimera