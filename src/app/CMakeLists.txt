add_executable(streaming_app WIN32
    main.cpp
    ui/main_window.cpp
    ui/main_window.hpp
    ui/webview_host.cpp
    ui/webview_host.hpp
    ui/window_base.cpp
    ui/window_base.hpp
)

target_link_libraries(streaming_app
    PRIVATE
        core
        domain
        services
        unofficial::webview2::webview2
        WIL::WIL
)