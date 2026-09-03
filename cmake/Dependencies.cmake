# External Dependencies Management
include(FetchContent)

# 1. nlohmann/json (Pinned Tag v3.11.3)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

# 2. Catch2 (Pinned Tag v3.5.3 for Unit Testing)
if(SPACE2X_BUILD_TESTS)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.3
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(Catch2)
endif()

# 3. Qt 6 (Optional for GUI)
if(SPACE2X_BUILD_UI)
    find_package(Qt6 6.4 REQUIRED COMPONENTS Core Widgets Network)
endif()

# 4. Linux specific dependencies
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBSYSTEMD libsystemd)
endif()
