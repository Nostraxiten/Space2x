cmake_minimum_required(VERSION 3.22)

project(Space2X
    VERSION 0.1.0
    DESCRIPTION "Cross-platform local infrastructure and service management platform"
    HOMEPAGE_URL "https://github.com/space2x/space2x"
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Target Windows 10+ API surface for all Windows builds.
# QueryServiceStatusProcess, GAA_FLAG_INCLUDE_GATEWAYS, etc. require >= 0x0600.
if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0A00 WINVER=0x0A00)
endif()

# Build Options
option(SPACE2X_BUILD_CLI "Build the Space2X command line interface" ON)
option(SPACE2X_BUILD_UI "Build the Space2X Qt6 desktop graphical interface" OFF)
option(SPACE2X_BUILD_TESTS "Build Space2X automated unit test suites" ON)
option(SPACE2X_INTEGRATION_TESTS "Enable live integration tests against host OS" OFF)
option(SPACE2X_ENABLE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)

# Module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CompilerOptions)
include(Dependencies)

# Core Library
add_subdirectory(src/core)

# CLI Interface
if(SPACE2X_BUILD_CLI)
    add_subdirectory(src/cli)
endif()

# GUI Interface (Qt6)
if(SPACE2X_BUILD_UI)
    add_subdirectory(src/ui)
endif()

# Unit Tests
if(SPACE2X_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Packaging
include(Packaging)
