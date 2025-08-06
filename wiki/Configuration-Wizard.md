# Configuration Wizard in Chimera

## Overview

Chimera's Configuration Wizard provides an interactive, user-friendly method to set up and configure the steganographic communication system. This wizard simplifies the complex configuration process, making the framework more accessible to users with varying technical expertise.

## Key Features

### 1. Interactive Configuration
- Guided, step-by-step configuration process
- Real-time input validation
- Comprehensive configuration options

### 2. Input Validation
- DNS Server IP validation
- Domain name format checking
- Transport layer selection
- Encoding strategy configuration

## Configuration Options

### DNS Server
- Validate IPv4 address format
- Support for major public DNS servers
- Custom DNS server configuration

### Target Domain
- Validate domain name syntax
- Support for various domain formats
- Flexibility in domain selection

### Transport Layer Selection
Available Options:
- DNS over HTTPS (DoH)
- DNS over TLS (DoT)
- Traditional UDP

### Encoding Strategy
Supported Strategies:
- Single Record Encoding
- Multi-Record Encoding

### Advanced Settings
- Compression toggle
- Noise ratio configuration
- Fragment management

## Usage Examples

### Basic Configuration
```cpp
#include "chimera/config_wizard.hpp"

// Initialize configuration wizard
chimera::ConfigWizard wizard;

// Run interactive setup
chimera::ClientConfig config = wizard.runInteractiveSetup();

// Use generated configuration
chimera::ChimeraClient client(config);
```

### Programmatic Configuration
```cpp
// If you prefer more control, manually set configuration
chimera::ClientConfig config;
config.dns_server = "8.8.8.8";
config.target_domain = "example.com";
config.transport = chimera::TransportType::DoH;
config.encoding_strategy = chimera::EncodingStrategy::MULTI_RECORD;
```

## Validation Process

1. **Input Capture**: Prompt user for configuration details
2. **Regex Validation**: Check input against predefined patterns
3. **Semantic Validation**: Ensure configuration makes logical sense
4. **Configuration Generation**: Create a validated `ClientConfig`

## Error Handling

- Clear, user-friendly error messages
- Guidance for correct input
- Retry mechanisms for invalid configurations

## Best Practices

1. Use wizard for initial configuration
2. Review generated configuration
3. Customize advanced settings as needed
4. Regenerate configuration if requirements change

## Performance Considerations

- Minimal runtime overhead
- Configuration generation typically < 100ms
- No persistent storage of configuration details

## Security Notes

- Configuration remains in-memory
- No external data transmission
- Sensitive information handled securely

## Extensibility

Developers can:
- Add custom validation rules
- Extend configuration options
- Implement additional configuration strategies

## Troubleshooting

If configuration wizard fails:
- Check input format
- Verify network and system settings
- Consult [Troubleshooting Guide](Troubleshooting.md)

## Advanced Usage

### Custom Validation
```cpp
// Example of adding custom validation
wizard.addValidationRule("custom_field", [](const std::string& value) {
    // Custom validation logic
    return !value.empty();
});
```

## Integration

Compatible with:
- Manual configuration
- Environment variable configuration
- External configuration management systems