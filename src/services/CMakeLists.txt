add_library(services   
cache/cache_manager.hpp
    cache/cache_manager.cpp
    cache/cache_manager.hpp

    media/media_service.hpp
    media/media_service.cpp
    media/IMediaProvider.hpp

    providers/GenericProvider.cpp
    providers/GenericProvider.hpp
    providers/provider_repository.cpp
    providers/provider_repository.hpp
 
    # auth/auth_manager.hpp
)

target_link_libraries(services
    PUBLIC
        core
        domain
        CURL::libcurl
        fmt::fmt
        nlohmann_json::nlohmann_json
)



target_include_directories(services
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)