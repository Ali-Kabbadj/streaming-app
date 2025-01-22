add_library(services
    media/media_service.hpp
    media/media_service.cpp
    media/IMediaProvider.hpp
    providers/omdbMediaProvider.cpp
    providers/omdbMediaProvider.hpp
    providers/tmdb_provider.hpp
    providers/tmdb_provider.cpp
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