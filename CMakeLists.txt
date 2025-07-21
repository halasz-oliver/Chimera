cmake_minimum_required(VERSION 3.16)
project(chimera VERSION 0.2.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSODIUM REQUIRED IMPORTED_TARGET libsodium)

# OpenSSL dependency for liboqs
find_package(OpenSSL REQUIRED)

# liboqs for post-quantum cryptography
find_library(LIBOQS_LIB oqs REQUIRED)
find_path(LIBOQS_INCLUDE_DIR oqs/oqs.h REQUIRED)

# libcurl for DoH (DNS-over-HTTPS) support
find_package(CURL REQUIRED)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Debug/Release specific settings
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(CHIMERA_DEBUG=1)
    add_compile_options(-g -O0)
else()
    add_compile_options(-O3 -DNDEBUG)
endif()

# Core library
add_library(chimera_core
        src/client.cpp
        src/dns_packet.cpp
        src/crypto.cpp
        src/Transport.cpp
        src/BehavioralMimicry.cpp
        src/AsyncIO.cpp
)

target_include_directories(chimera_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${LIBOQS_INCLUDE_DIR}
)

target_link_libraries(chimera_core PUBLIC
        PkgConfig::LIBSODIUM
        ${LIBOQS_LIB}
        OpenSSL::SSL
        OpenSSL::Crypto
        CURL::libcurl
)

# Platform specific links
if(UNIX AND NOT APPLE)
    target_link_libraries(chimera_core PRIVATE pthread)
elseif(WIN32)
    target_link_libraries(chimera_core PRIVATE ws2_32)
endif()

# Main executable
add_executable(chimera_demo src/main.cpp)
target_link_libraries(chimera_demo chimera_core)

# Test executable
add_executable(chimera_test tests/test_basic.cpp)
target_link_libraries(chimera_test chimera_core)

# Comprehensive test executable (Phase 1 + 2)
add_executable(chimera_test_comprehensive tests/test_comprehensive.cpp)
target_link_libraries(chimera_test_comprehensive chimera_core)

# Custom targets
add_custom_target(run_tests
        COMMAND chimera_test
        DEPENDS chimera_test
        COMMENT "Running basic tests"
)

add_custom_target(run_comprehensive_tests
        COMMAND chimera_test_comprehensive
        DEPENDS chimera_test_comprehensive
        COMMENT "Running comprehensive Phase 1 + 2 tests"
)

add_custom_target(run_all_tests
        COMMAND chimera_test && chimera_test_comprehensive
        DEPENDS chimera_test chimera_test_comprehensive
        COMMENT "Running all test suites"
)

add_custom_target(run_demo
        COMMAND chimera_demo
        DEPENDS chimera_demo
        COMMENT "Running demo application"
)
