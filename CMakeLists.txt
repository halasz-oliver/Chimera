cmake_minimum_required(VERSION 3.15)
project(chimera VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    # TODO: -Werror hozzáadása amikor minden warning el van javítva
endif()

# Debug/Release specifikus beállítások
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(CHIMERA_DEBUG=1)
    add_compile_options(-g -O0)
else()
    add_compile_options(-O3 -DNDEBUG)
    # TODO: LTO engedélyezés release buildekhez
endif()

# Core library
add_library(chimera_core
        src/client.cpp
        src/dns_packet.cpp
)

target_include_directories(chimera_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Platform specifikus linkek
if(UNIX AND NOT APPLE)
    # Linux specifikus
    target_link_libraries(chimera_core PRIVATE pthread)
    # TODO: liburing support hozzáadása
elseif(APPLE)
    # macOS specifikus
    # TODO: kqueue support
elseif(WIN32)
    # Windows specifikus
    target_link_libraries(chimera_core PRIVATE ws2_32)
    # TODO: IOCP support
endif()

# Main executable
add_executable(chimera_demo src/main.cpp)
target_link_libraries(chimera_demo chimera_core)

# Test executable
add_executable(chimera_test tests/test_basic.cpp)
target_link_libraries(chimera_test chimera_core)

# TODO: Install targets
# TODO: Package generation (CPack)
# TODO: Documentation generation (Doxygen)

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

# TODO: Valgrind target memória leak ellenőrzéshez
# TODO: Clang-tidy target static analysis-hez
