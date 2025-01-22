add_library(domain INTERFACE)

target_include_directories(domain
    INTERFACE
        ${CMAKE_SOURCE_DIR}/src
)