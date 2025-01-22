# Add subdirectories
add_subdirectory(app)
add_subdirectory(core)
add_subdirectory(services)
add_subdirectory(domain)

# Main executable
add_executable(streaming_app WIN32
    app/main.cpp
)

target_link_libraries(streaming_app
    PRIVATE
    app_ui
    core_ipc
    core_utils
    core_window
    services_media
    domain_models
)

# Set the working directory for Visual Studio
set_property(TARGET streaming_app PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>")