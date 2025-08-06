# Examples

## Basic
```cpp
#include "chimera/client.hpp"
chimera::ClientConfig c;
c.dns_server = "8.8.8.8";
c.target_domain = "example.com";
chimera::ChimeraClient cl(c);
auto r = cl.send_text("Hello");
```

## Fallback across transports
```cpp
chimera::ClientConfig base;
base.dns_server = "8.8.8.8";
base.target_domain = "example.com";
for (auto t : {chimera::TransportType::DoH,
               chimera::TransportType::DoT,
               chimera::TransportType::UDP}) {
  auto cfg = base;
  cfg.transport = t;
  chimera::ChimeraClient cl(cfg);
  if (cl.send_text("msg")) break;
}
```

## Wizard + client
```cpp
#include "chimera/config_wizard.hpp"
chimera::ConfigWizard w;
auto cfg = w.runInteractiveSetup();
chimera::ChimeraClient cl(cfg);
```

## Diagnostics report
```cpp
#include "chimera/system_diagnostics.hpp"
std::cout << chimera::SystemDiagnostics::generateDetailedReport();
```

## Steganography options
```cpp
chimera::ClientConfig c;
c.encoding_strategy = chimera::EncodingStrategy::MULTI_RECORD;
c.use_compression = true;
c.noise_ratio = 0.1;
c.max_fragments = 5;
```

## Transport choices
```cpp
chimera::ClientConfig c;
c.transport = chimera::TransportType::UDP; // or DoH/DoT
```

## Tests
```bash
cmake --build build --target run_tests
build/chimera_test --all
```
