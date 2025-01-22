add_library(domain INTERFACE)


target_link_libraries(domain
    INTERFACE
        date::date
)

target_include_directories(domain
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
    
)

