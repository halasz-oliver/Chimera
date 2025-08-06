# Configuration Wizard

## Overview
Interactive CLI to build a validated chimera::ClientConfig.
clear prompts and retries.

## What it asks
- DNS server: accepts IPv4, IPv6, or hostname; regex validated
- Target domain: validated against domain regex
- Transport: choose 1..N from DoH, DoT, UDP (with descriptions)
- Encoding: Single Record or Multi-Record
- Advanced: compression (y/n), noise ratio [0.0..1.0]

## Usage
```cpp
#include "chimera/config_wizard.hpp"
chimera::ConfigWizard wizard;
chimera::ClientConfig config = wizard.runInteractiveSetup();
```

## Output mapping
- Transport: DoH/DoT/UDP → TransportType::{DoH,DoT,UDP}
- Encoding: Single/Multi → EncodingStrategy::{SINGLE_RECORD,MULTI_RECORD}
- Compression: y/n → bool
- Noise ratio: double clamped to [0,1] via validator

## Summary
- Prints human-readable summary with transportToString and
  encodingToString
- Shows compression Enabled/Disabled and noise ratio with 2 decimals

## Extending
- Add new validation in initializeValidationRules()
- Add new prompt and update summary accordingly

## Notes
- Wizard does not persist to disk; caller owns saving config
- See README for build/test targets to run the demo and tests
