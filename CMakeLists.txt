cmake_minimum_required(VERSION 3.25)
project(CompressionMath VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
elseif(MSVC)
    add_compile_options(/W4 /O2)
endif()

# ============================================================================
# Library (header-only)
# ============================================================================
add_library(compression_math INTERFACE)
add_library(compression_math::compression_math ALIAS compression_math)

target_include_directories(compression_math INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# ============================================================================
# Demo executable
# ============================================================================
add_executable(demo_compression_math src/main.cpp)
target_link_libraries(demo_compression_math PRIVATE compression_math::compression_math)

# ============================================================================
# Test executable
# ============================================================================
add_executable(test_compression_math tests/test_expansion.cpp)
target_link_libraries(test_compression_math PRIVATE compression_math::compression_math)

# ============================================================================
# CTest integration
# ============================================================================
enable_testing()
add_test(NAME expansion_tests COMMAND test_compression_math)

# ============================================================================
# Install rules
# ============================================================================
include(GNUInstallDirs)
install(DIRECTORY include/compression_math DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS compression_math EXPORT compression_math-targets)
install(EXPORT compression_math-targets
    FILE compression_math-targets.cmake
    NAMESPACE compression_math::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/compression_math
)
