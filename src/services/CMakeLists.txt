add_library(services
    media/media_service.hpp
    media/media_service.cpp
    providers/omdbMediaProvider.hpp
    providers/omdbMediaProvider.cpp
    providers/tmdb_provider.hpp
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