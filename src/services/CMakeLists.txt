add_library(services
    media/media_service.hpp
    media/media_service.cpp
    media/IMediaProvider.hpp
    providers/omdbMediaProvider.cpp
    providers/omdbMediaProvider.hpp
    providers/tmdb_provider.hpp
    providers/tmdb_provider.cpp
    providers/provider_repository.cpp
    providers/provider_repository.hpp
    providers/utils/rating_normalizer.hpp
    providers/utils/rating_normalizer.cpp
    cache/cache_manager.hpp
    cache/cache_manager.cpp
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