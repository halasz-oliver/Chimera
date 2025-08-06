# System Diagnostics

## Overview
Chimera's SystemDiagnostics provides actionable checks to validate
environment readiness and surface concrete remediation steps.

## What it checks
- Cryptographic libraries: verifies availability of libsodium and liboqs
  by attempting to load common library names across platforms
- Network: runs a DNS resolution sanity test (nslookup or dig) and flags
  empty results
- Performance: inspects ru_maxrss via getrusage, with platform-correct
  units (bytes on macOS, KiB on Linux)
- File permissions: verifies an executable path is permitted
  (platform-specific check)
- Severity sorting: results are ordered by severity (CRITICAL → INFO)
  for at-a-glance triage

## Usage
```cpp
#include "chimera/system_diagnostics.hpp"
auto reports = chimera::SystemDiagnostics::runPreflightChecks();
std::cout << chimera::SystemDiagnostics::generateDetailedReport();
```

## Output format
- Header with human-readable timestamp
- Per-check lines: [LEVEL] message, optional Suggestion: ...
- Levels: INFO, WARNING, ERROR, CRITICAL

## Typical results
- INFO: Cryptographic libraries verified
- WARNING: DNS resolution returned no records (Suggestion: Check outbound
  DNS/DoH/DoT connectivity)
- WARNING: High memory consumption detected (Suggestion: Optimize memory
  or upgrade resources)
- ERROR/CRITICAL: Missing libraries (Suggestion: install libsodium and
  liboqs)

## Platform notes
- macOS: ru_maxrss interpreted as bytes; Homebrew installs may require
  CMAKE_PREFIX_PATH for curl/openssl
- Linux: ru_maxrss interpreted as KiB; /proc is available, permission
  check uses /proc/self/exe
- Windows: dynamic library probe uses names like sodium.dll/oqs.dll when
  present

## Extending
- Add a new private check method returning DiagnosticReport
- Append it in runPreflightChecks(); it will be auto-sorted by severity

## Security
- No secrets collected; no external transmission
- Logs use [CHIMERA LEVEL] prefix without sensitive data
