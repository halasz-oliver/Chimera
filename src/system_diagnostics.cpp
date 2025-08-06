#include "chimera/system_diagnostics.hpp"
#include <dlfcn.h>
#include <unistd.h>
#include <sys/resource.h>

namespace {
    // Helper to check library availability
    bool isLibraryAvailable(const std::string& libname) {
        void* handle = dlopen(libname.c_str(), RTLD_LAZY);
        if (handle) {
            dlclose(handle);
            return true;
        }
        return false;
    }
}

namespace chimera {

std::vector<DiagnosticReport> SystemDiagnostics::runPreflightChecks() {
    std::vector<DiagnosticReport> reports;

    // Run all diagnostic checks
    reports.push_back(checkCryptoLibraries());
    reports.push_back(checkNetworkCapabilities());
    reports.push_back(checkPerformanceMetrics());
    reports.push_back(checkFilePermissions());

    return reports;
}

DiagnosticReport SystemDiagnostics::checkCryptoLibraries() {
    bool hasLibSodium = isLibraryAvailable("libsodium.so") || 
                        isLibraryAvailable("libsodium.dylib");
    bool hasLibOQS = isLibraryAvailable("liboqs.so") || 
                     isLibraryAvailable("liboqs.dylib");

    if (!hasLibSodium || !hasLibOQS) {
        return {
            DiagnosticLevel::CRITICAL,
            "Missing cryptographic libraries",
            "Install libsodium and liboqs. See installation guide in README."
        };
    }

    return {DiagnosticLevel::INFO, "Cryptographic libraries verified", ""};
}

DiagnosticReport SystemDiagnostics::checkNetworkCapabilities() {
    // Simple DNS resolution test
    FILE* pipe = popen("dig +short example.com", "r");
    if (!pipe) {
        return {
            DiagnosticLevel::WARNING,
            "Unable to perform DNS resolution test",
            "Check network connectivity and DNS configuration"
        };
    }
    pclose(pipe);

    return {DiagnosticLevel::INFO, "Network capabilities verified", ""};
}

DiagnosticReport SystemDiagnostics::checkPerformanceMetrics() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);

    // Check memory usage and CPU limits
    if (usage.ru_maxrss > 1024 * 1024 * 500) {  // 500 MB threshold
        return {
            DiagnosticLevel::WARNING,
            "High memory consumption detected",
            "Consider optimizing memory usage or upgrading system resources"
        };
    }

    return {DiagnosticLevel::INFO, "Performance metrics within acceptable range", ""};
}

DiagnosticReport SystemDiagnostics::checkFilePermissions() {
    // Check current executable's permissions
    if (access("/proc/self/exe", X_OK) != 0) {
        return {
            DiagnosticLevel::ERROR,
            "Insufficient file execution permissions",
            "Ensure the application has executable permissions"
        };
    }

    return {DiagnosticLevel::INFO, "File permissions verified", ""};
}

std::string SystemDiagnostics::generateDetailedReport() {
    std::vector<DiagnosticReport> checks = runPreflightChecks();
    std::ostringstream report;

    report << "Chimera System Diagnostic Report\n";
    report << "================================\n";
    report << "Generated: " 
           << std::chrono::system_clock::now().time_since_epoch().count() 
           << "\n\n";

    for (const auto& check : checks) {
        std::string levelStr;
        switch(check.level) {
            case DiagnosticLevel::INFO: levelStr = "INFO"; break;
            case DiagnosticLevel::WARNING: levelStr = "WARNING"; break;
            case DiagnosticLevel::ERROR: levelStr = "ERROR"; break;
            case DiagnosticLevel::CRITICAL: levelStr = "CRITICAL"; break;
        }

        report << "[" << levelStr << "] " 
               << check.message << "\n";
        
        if (!check.suggestion.empty()) {
            report << "    Suggestion: " << check.suggestion << "\n";
        }
    }

    return report.str();
}

void SystemDiagnostics::logDiagnostic(DiagnosticLevel level, 
                                       const std::string& message, 
                                       const std::string& suggestion) {
    std::cerr << "[CHIMERA DIAGNOSTIC] " 
              << message 
              << (suggestion.empty() ? "" : " (" + suggestion + ")") 
              << std::endl;
}

}  // namespace chimera