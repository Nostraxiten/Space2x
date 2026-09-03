set(CLI_SOURCES
    main.cpp
    commands/ServicesCommand.cpp
    commands/ServicesCommand.h
    commands/ProfilesCommand.cpp
    commands/ProfilesCommand.h
    commands/SystemCommand.cpp
    commands/SystemCommand.h
    commands/NetworkCommand.cpp
    commands/NetworkCommand.h
)

if(WIN32)
    list(APPEND CLI_SOURCES ${CMAKE_SOURCE_DIR}/src/ui/space2x.rc)
endif()

add_executable(space2x ${CLI_SOURCES})


target_include_directories(space2x
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
)

space2x_apply_target_options(space2x)

target_link_libraries(space2x
    PRIVATE
        space2x-core
)

install(TARGETS space2x
    RUNTIME DESTINATION bin
)
