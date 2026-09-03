set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(space2x-ui
    main.cpp
    MainWindow.cpp
    MainWindow.h
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
)

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

install(TARGETS space2x-ui
    RUNTIME DESTINATION bin
)
