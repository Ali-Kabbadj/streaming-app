set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
if(MSVC)
    add_compile_options(/WX-)
endif()


# Add subdirectories
add_subdirectory(core)
add_subdirectory(domain)
add_subdirectory(services)
add_subdirectory(app)