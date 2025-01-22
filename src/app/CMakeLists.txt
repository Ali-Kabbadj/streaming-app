if(MSVC)
    add_compile_options(/WX-)
endif()

add_library(app_ui
    ui/main_window.cpp
    ui/main_window.hpp
    ui/webview_host.cpp
    ui/webview_host.hpp
    ui/window_base.cpp
    ui/window_base.hpp
)

target_link_libraries(app_ui
    PUBLIC
        core
        domain
        services
        unofficial::webview2::webview2
        WIL::WIL
)

add_executable(streaming_app WIN32
    main.cpp
)

target_link_libraries(streaming_app
    PRIVATE
        app_ui
)