add_executable(harness_tests
    test_smoke.cpp
)

target_link_libraries(harness_tests PRIVATE
    Catch2::Catch2WithMain
    harness_lib
    harness_warnings
)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(harness_tests)
