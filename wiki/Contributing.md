# Contributing to CHIMERA

Thank you for your interest in contributing to CHIMERA! This guide will help you get started with development and contributions.

## Project Status

**All three phases of CHIMERA are complete and production-ready!** We welcome contributions for:
- Performance optimizations
- Additional transport protocols
- Enhanced evasion techniques
- Documentation improvements
- Platform-specific optimizations
- Bug fixes and improvements

## Development Setup

### Prerequisites

- **C++20** compatible compiler (GCC 8+, Clang 10+, MSVC 2019+)
- **CMake 3.16+**
- **Git** for version control
- **Dependencies**: libsodium, liboqs, libcurl, OpenSSL, zlib

### Setting Up Development Environment

1. **Clone the repository**:
   ```bash
   git clone https://github.com/halasz-oliver/chimera.git
   cd chimera
   ```

2. **Install dependencies** (see [Getting Started](Getting-Started.md) for platform-specific instructions)

3. **Create development build**:
   ```bash
   mkdir build && cd build
   cmake -DCMAKE_BUILD_TYPE=Debug ..
   make -j$(nproc)
   ```

4. **Run tests to verify setup**:
   ```bash
   ./chimera_test --all
   ```

### Development Tools

For consistent code style and quality:

```bash
# Install development tools (Ubuntu/Debian)
sudo apt install clang-format cppcheck doxygen

# Install development tools (macOS)
brew install clang-format cppcheck doxygen

# Format code
clang-format -i src/*.cpp include/chimera/*.hpp

# Static analysis
cppcheck --enable=all --std=c++20 src/ include/
```

## Code Style Guidelines

### C++ Standards

- **C++20** features are encouraged
- Use **modern C++** idioms (RAII, smart pointers, etc.)
- Prefer **standard library** over custom implementations
- Use **tl::expected** for error handling

### Naming Conventions

```cpp
// Classes: PascalCase
class ChimeraClient {};

// Functions and variables: snake_case
void send_message();
int message_count = 0;

// Constants: UPPER_SNAKE_CASE
const int MAX_FRAGMENTS = 10;

// Enums: PascalCase with scoped enums
enum class TransportType { UDP, DoH, DoT };

// Private members: trailing underscore
class Example {
private:
    int private_member_;
};
```

### File Organization

```
include/chimera/     # Public headers
├── client.hpp       # Main client interface
├── crypto.hpp       # Cryptographic functions
├── Transport.hpp    # Transport abstractions
└── ...

src/                 # Implementation files
├── client.cpp
├── crypto.cpp
├── Transport.cpp
└── ...

tests/              # Test files
├── test_unified.cpp # Main test suite
└── ...

wiki/               # Documentation
├── Home.md
├── API-Reference.md
└── ...
```

## Testing

### Running Tests

```bash
# Run all tests
./chimera_test --all

# Run specific categories
./chimera_test --core
./chimera_test --transport
./chimera_test --steganography
./chimera_test --integration
./chimera_test --performance

# Quick tests for development
./chimera_test --quick
```

### Writing Tests

Add tests to the unified test suite in `tests/test_unified.cpp`:

```cpp
void test_new_feature(TestRunner& runner) {
    runner.run_test("Category", "Test Name", []() {
        // Test implementation
        assert(condition);
        
        // Use std::cout for test output
        std::cout << "Test details..." << std::endl;
    });
}

// Add to main() function in appropriate section
if (run_all || run_category) {
    chimera::tests::test_new_feature(runner);
}
```

### Test Categories

- **Core**: Basic functionality (crypto, DNS, encoding)
- **Transport**: Network transport layers
- **Steganography**: Phase 3 advanced encoding
- **Integration**: End-to-end functionality
- **Performance**: Benchmarks and optimization

## Contribution Areas

### 1. Performance Optimizations

Areas for improvement:
- Encoding/decoding algorithms
- Memory allocation patterns
- Network I/O efficiency
- Cryptographic operations

Example contribution:
```cpp
// Optimize encoding performance
std::vector<uint8_t> optimized_encode(const std::vector<uint8_t>& data) {
    // Implementation with better performance
    // Include benchmarks in tests
}
```

### 2. Additional Transport Protocols

Potential new transports:
- **QUIC/HTTP3**: Next-generation protocol
- **WebRTC**: Peer-to-peer communication
- **Custom protocols**: Domain-specific transports

Implementation pattern:
```cpp
class TransportQUIC : public ITransport {
public:
    tl::expected<size_t, TransportError> send(const std::vector<uint8_t>& data) override;
    tl::expected<std::vector<uint8_t>, TransportError> receive() override;
    void set_timeout(std::chrono::milliseconds timeout) override;
};
```

### 3. Enhanced Evasion Techniques

Areas for improvement:
- Traffic pattern analysis
- Timing randomization
- Protocol mimicry
- Detection avoidance

### 4. Platform Support

Platform-specific optimizations:
- **Windows**: IOCP async I/O
- **Linux**: epoll optimizations
- **macOS**: kqueue improvements
- **Mobile**: Android/iOS support

### 5. Documentation

Documentation improvements:
- API documentation
- Usage examples
- Performance guides
- Security analysis

## Submission Process

### 1. Issue Discussion

Before major changes:
1. **Open an issue** describing the proposed change
2. **Discuss approach** with maintainers
3. **Get approval** for significant modifications

### 2. Development Process

1. **Create feature branch**:
   ```bash
   git checkout -b feature/description
   ```

2. **Implement changes** following code style guidelines

3. **Add tests** for new functionality

4. **Update documentation** as needed

5. **Verify all tests pass**:
   ```bash
   ./chimera_test --all
   ```

### 3. Pull Request

1. **Create pull request** with clear description
2. **Include test results** and performance impact
3. **Reference related issues**
4. **Respond to review feedback**

### Pull Request Template

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Performance improvement
- [ ] Documentation update
- [ ] Breaking change

## Testing
- [ ] All existing tests pass
- [ ] New tests added for new functionality
- [ ] Performance impact measured

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Documentation updated
- [ ] Tests added/updated
```

## Security Considerations

### Cryptographic Changes

For cryptographic modifications:
- **Use established libraries** (libsodium, liboqs)
- **Include security analysis** in documentation
- **Get expert review** for significant changes
- **Maintain backward compatibility** when possible

### Network Security

For network-related changes:
- **Consider traffic analysis** implications
- **Test with various network conditions**
- **Document security properties**
- **Include evasion effectiveness analysis**

## Performance Guidelines

### Benchmarking

Include performance measurements:
```cpp
void test_performance_improvement(TestRunner& runner) {
    runner.run_test("Performance", "Improved Algorithm", []() {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Test implementation
        for (int i = 0; i < 1000; ++i) {
            improved_algorithm();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "Performance: " << duration.count() << " μs for 1000 operations" << std::endl;
        
        // Compare with baseline if available
        assert(duration.count() < baseline_duration);
    });
}
```

### Memory Usage

Monitor memory usage:
- Use **RAII** for automatic cleanup
- Avoid **memory leaks**
- Profile with **valgrind** or similar tools
- Document **memory requirements**

## Release Process

### Version Numbering

CHIMERA uses semantic versioning:
- **Major.Minor.Patch** (e.g., 0.2.1)
- **Major**: Breaking changes
- **Minor**: New features
- **Patch**: Bug fixes

### Release Checklist

- [ ] All tests pass
- [ ] Documentation updated
- [ ] Performance benchmarks run
- [ ] Security review completed
- [ ] Version number updated

## Getting Help

### Communication Channels

- **Issues**: GitHub issues for bugs and feature requests
- **Email**: `halaszoliver45(at)gmail.com` for direct contact
- **Documentation**: Wiki for comprehensive guides

### Questions and Support

For questions about:
- **Development setup**: See [Getting Started](Getting-Started.md)
- **API usage**: See [API Reference](API-Reference.md)
- **Configuration**: See [Configuration](Configuration.md)
- **Troubleshooting**: See [Troubleshooting](Troubleshooting.md)

## License

By contributing to CHIMERA, you agree that your contributions will be licensed under the Apache License 2.0.

---

**Thank you for contributing to CHIMERA!** Your contributions help make secure, quantum-resistant steganographic communication available to everyone.