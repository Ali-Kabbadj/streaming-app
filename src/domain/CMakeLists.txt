add_library(domain INTERFACE)



target_include_directories(domain
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
    
)

