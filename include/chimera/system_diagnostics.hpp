#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace chimera {

enum class DiagnosticLevel {
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

struct DiagnosticReport {
    DiagnosticLevel level;
    std::string message;
    std::string suggestion;
};

class SystemDiagnostics {
public:
    // Check system compatibility and readiness
    static std::vector<DiagnosticReport> runPreflightChecks();

    // Generate a comprehensive system diagnostic report
    static std::string generateDetailedReport();

    // Log diagnostic information
    static void logDiagnostic(DiagnosticLevel level, 
                               const std::string& message, 
                               const std::string& suggestion = "");

private:
    // Check cryptographic library availability
    static DiagnosticReport checkCryptoLibraries();

    // Check network capabilities
    static DiagnosticReport checkNetworkCapabilities();

    // Verify system performance metrics
    static DiagnosticReport checkPerformanceMetrics();

    // Check file and directory permissions
    static DiagnosticReport checkFilePermissions();
};

}  // namespace chimera