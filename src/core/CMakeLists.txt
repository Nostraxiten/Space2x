add_library(space2x-core STATIC
    AuditLog.cpp
    AuditLog.h
    VersionStore.cpp
    VersionStore.h
    ConfigManager.cpp
    ConfigManager.h
    ProfileManager.cpp
    ProfileManager.h
    ServiceController.cpp
    ServiceController.h
    Engine.cpp
    Engine.h

    # Windows platform implementations
    ../platform/windows/WinServiceManager.cpp
    ../platform/windows/WinServiceManager.h
    ../platform/windows/WinProcessManager.cpp
    ../platform/windows/WinProcessManager.h
    ../platform/windows/WinSystemMonitor.cpp
    ../platform/windows/WinSystemMonitor.h
    ../platform/windows/WinNetworkManager.cpp
    ../platform/windows/WinNetworkManager.h
    ../platform/windows/WinPackageManager.cpp
    ../platform/windows/WinPackageManager.h

    # Linux platform implementations
    ../platform/linux/LinuxServiceManager.cpp
    ../platform/linux/LinuxServiceManager.h
    ../platform/linux/LinuxProcessManager.cpp
    ../platform/linux/LinuxProcessManager.h
    ../platform/linux/LinuxSystemMonitor.cpp
    ../platform/linux/LinuxSystemMonitor.h
    ../platform/linux/LinuxNetworkManager.cpp
    ../platform/linux/LinuxNetworkManager.h
    ../platform/linux/LinuxPackageManager.cpp
    ../platform/linux/LinuxPackageManager.h

    # Core Providers
    ../providers/openssh/OpenSSHProvider.cpp
    ../providers/openssh/OpenSSHProvider.h
    ../providers/postgresql/PostgreSQLProvider.cpp
    ../providers/postgresql/PostgreSQLProvider.h
    ../providers/mysql/MySQLProvider.cpp
    ../providers/mysql/MySQLProvider.h
    ../providers/nginx/NginxProvider.cpp
    ../providers/nginx/NginxProvider.h
    ../providers/redis/RedisProvider.cpp
    ../providers/redis/RedisProvider.h
)

target_include_directories(space2x-core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

space2x_apply_target_options(space2x-core)

target_link_libraries(space2x-core
    PUBLIC
        nlohmann_json::nlohmann_json
)

if(WIN32)
    target_link_libraries(space2x-core
        PRIVATE
            advapi32
            iphlpapi
            ws2_32
            psapi
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(LIBSYSTEMD_FOUND)
        target_link_libraries(space2x-core PRIVATE ${LIBSYSTEMD_LIBRARIES})
    endif()
endif()
