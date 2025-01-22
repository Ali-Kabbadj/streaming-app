add_library(services
    media/media_service.hpp
    media/mock_media_provider.cpp
)

target_link_libraries(services
    PUBLIC
        core
        domain
)

target_include_directories(services
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)