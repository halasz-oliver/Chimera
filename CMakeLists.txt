cmake_minimum_required(VERSION 3.16)
project(chimera VERSION 0.2.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSODIUM REQUIRED IMPORTED_TARGET libsodium)

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
)

target_include_directories(chimera_core PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(chimera_core PUBLIC PkgConfig::LIBSODIUM)

# Platform specific links (excluding libsodium, now handled above)
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

# Custom targets
add_custom_target(run_tests
COMMAND chimera_test
DEPENDS chimera_test
COMMENT "Running comprehensive tests"
)

add_custom_target(run_demo
COMMAND chimera_demo
DEPENDS chimera_demo
COMMENT "Running demo application"
)
