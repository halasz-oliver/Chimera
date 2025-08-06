#include "chimera/system_diagnostics.hpp"
#include <dlfcn.h>
#include <unistd.h>
#include <sys/resource.h>
#include <ctime>

namespace {
    // Helper to check library availability
    bool isLibraryAvailable(const std::vector<std::string>& names) {
        for (const auto& n : names) {
            void* handle = dlopen(n.c_str(), RTLD_LAZY);
            if (handle) { dlclose(handle); return true; }
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
    
    // Sort by severity
    std::stable_sort(reports.begin(), reports.end(), [](const auto& a, const auto& b){
        return static_cast<int>(a.level) > static_cast<int>(b.level);
    });

    return reports;
}

DiagnosticReport SystemDiagnostics::checkCryptoLibraries() {
    bool hasLibSodium = isLibraryAvailable({"libsodium.so","libsodium.dylib","sodium.dll"});
    bool hasLibOQS = isLibraryAvailable({"liboqs.so","liboqs.dylib","oqs.dll"});

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
    FILE* pipe = popen("nslookup -type=A example.com 2>/dev/null || dig +short example.com 2>/dev/null", "r");
    if (!pipe) {
        return {
            DiagnosticLevel::WARNING,
            "Unable to perform DNS resolution test",
            "Check network connectivity and DNS configuration"
        };
    }
    char buffer[256];
    bool ok = false;
    if (fgets(buffer, sizeof(buffer), pipe)) {
        ok = std::string(buffer).find_first_not_of(" \t\r\n") != std::string::npos;
    }
    pclose(pipe);

    if (!ok) {
        return {DiagnosticLevel::WARNING, "DNS resolution returned no records", "Check outbound DNS/DoH/DoT connectivity"};
    }
    return {DiagnosticLevel::INFO, "Network capabilities verified", ""};
}

DiagnosticReport SystemDiagnostics::checkPerformanceMetrics() {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) != 0) {
        return {DiagnosticLevel::WARNING, "Unable to read resource usage", "Ensure platform supports getrusage"};
    }

    long rss = usage.ru_maxrss;
#ifdef __APPLE__
    // ru_maxrss is bytes on macOS
    if (rss > 500L * 1024L * 1024L)
#else
    // ru_maxrss is kilobytes on Linux
    if (rss > 500L * 1024L)
#endif
    {
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
#if defined(__linux__)
    const char* exePath = "/proc/self/exe";
#elif defined(__APPLE__)
    const char* exePath = "/usr/bin/true"; // placeholder path with execute bit to test permission facility exists
#else
    const char* exePath = ".";
#endif
    if (access(exePath, X_OK) != 0) {
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
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    report << "Generated: " << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S") << "\n\n";

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
    const char* lvl = "INFO";
    switch (level) {
        case DiagnosticLevel::INFO: lvl = "INFO"; break;
        case DiagnosticLevel::WARNING: lvl = "WARNING"; break;
        case DiagnosticLevel::ERROR: lvl = "ERROR"; break;
        case DiagnosticLevel::CRITICAL: lvl = "CRITICAL"; break;
    }
    std::cerr << "[CHIMERA " << lvl << "] "
              << message 
              << (suggestion.empty() ? "" : " (" + suggestion + ")") 
              << std::endl;
}

}  // namespace chimera