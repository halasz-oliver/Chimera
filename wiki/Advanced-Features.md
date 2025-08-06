# Advanced Features

## Overview
Behavioral mimicry, adaptive transport, and steganography options improve
stealth. APIs may vary; align with headers in include/chimera/.

## Behavioral mimicry
```cpp
#include "chimera/BehavioralMimicry.hpp"
chimera::BehavioralMimicry m(chimera::BehavioralProfile::WebBrowsing);
auto delay = m.get_next_delay();
```
Profiles: Normal, WebBrowsing, Enterprise, Gaming, Random. Use
ClientConfig.timing_variance and ClientConfig.behavioral_profile for
simple tuning.

## Adaptive transport
Choose transport via ClientConfig::transport. If you implement dynamic
switching, gate it via adaptive_transport and your own manager.

## Async I/O
If async wrappers exist in your codebase, follow their header signatures;
otherwise prefer synchronous ChimeraClient for now.

## Steganography controls
- encoding_strategy: SINGLE_RECORD or MULTI_RECORD
- use_compression: enable zlib compression
- randomize_fragments: shuffle order
- noise_ratio: inject noise fragments [0..1]
- max_fragments: cap fragment count

## Example pattern
```cpp
chimera::ClientConfig c;
c.transport = chimera::TransportType::DoH;
c.behavioral_profile = chimera::BehavioralProfile::WebBrowsing;
c.encoding_strategy = chimera::EncodingStrategy::MULTI_RECORD;
```

## Performance tips
- UDP is fastest; DoH/DoT provide stealth
- Reduce timing_variance for throughput
- Keep fragments small; enable compression

## Notes
- Verify feature availability in headers before use
- See README and Configuration.md for definitive structures and enums
