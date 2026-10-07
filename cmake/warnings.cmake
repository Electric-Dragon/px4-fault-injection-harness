add_library(harness_warnings INTERFACE)

target_compile_options(harness_warnings INTERFACE
    -Wall
    -Werror
    -Wextra
    -Wpedantic
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Woverloaded-virtual
    -Wconversion
    -Wsign-conversion
    -Wnull-dereference
    -Wformat=2
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(harness_warnings INTERFACE
        -Wduplicated-cond
        -Wduplicated-branches
        -Wlogical-op
    )
endif()
