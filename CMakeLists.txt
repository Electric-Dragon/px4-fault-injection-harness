cmake_minimum_required(VERSION 3.25)
project(px4-fault-harness VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(HARNESS_BUILD_TESTS "Build unit tests" ON)

include(cmake/warnings.cmake)
include(cmake/dependencies.cmake)
include(cmake/mavsdk.cmake)

add_library(harness_lib INTERFACE)
target_include_directories(harness_lib INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(harness_lib INTERFACE
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    yaml-cpp::yaml-cpp
    CLI11::CLI11
    MAVSDK::mavsdk
)

add_executable(harness src/main.cpp)
target_link_libraries(harness PRIVATE harness_lib harness_warnings)

if(HARNESS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
