set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(UI_SOURCES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    ThemeManager.cpp
    ThemeManager.h
    widgets/StatusBadge.cpp
    widgets/StatusBadge.h
    widgets/ConfirmDialog.cpp
    widgets/ConfirmDialog.h
    views/DashboardView.cpp
    views/DashboardView.h
    views/ServicesView.cpp
    views/ServicesView.h
    views/DatabasesView.cpp
    views/DatabasesView.h
    views/ProcessesView.cpp
    views/ProcessesView.h
    views/NetworkView.cpp
    views/NetworkView.h
    views/LogsView.cpp
    views/LogsView.h
    views/SettingsView.cpp
    views/SettingsView.h
    resources.qrc
)

if(WIN32)
    list(APPEND UI_SOURCES space2x.rc)
endif()

add_executable(space2x-ui WIN32 ${UI_SOURCES})

target_include_directories(space2x-ui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
)

space2x_apply_target_options(space2x-ui)

target_link_libraries(space2x-ui
    PRIVATE
        space2x-core
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
)

if(WIN32)
    # Automatically deploy Qt runtime dependencies beside the executable
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET space2x-ui POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                    --verbose 1
                    --compiler-runtime
                    "$<TARGET_FILE:space2x-ui>"
            COMMENT "Deploying Qt libraries and dependencies for space2x-ui..."
        )
    endif()
endif()

install(TARGETS space2x-ui
    RUNTIME DESTINATION bin
)

