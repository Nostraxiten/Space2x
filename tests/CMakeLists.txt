add_executable(space2x-tests
    unit/core/test_result.cpp
    unit/core/test_error.cpp
    unit/core/test_service_controller.cpp
    unit/core/test_config_manager.cpp
    unit/core/test_profile_manager.cpp
    unit/providers/test_provider_registry.cpp
)

target_include_directories(space2x-tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)

space2x_apply_target_options(space2x-tests)

target_link_libraries(space2x-tests
    PRIVATE
        space2x-core
        Catch2::Catch2WithMain
)

include(CTest)
add_test(NAME Space2XUnitTests COMMAND space2x-tests)
