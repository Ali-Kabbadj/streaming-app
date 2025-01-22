if(MSVC)
    add_compile_options(/WX-)
endif()

add_library(core
    utils/logger.cpp
    utils/logger.hpp
    utils/result.hpp
    utils/win32_utils.hpp
    utils/win32_utils.cpp
    utils/result.hpp
    utils/http_client.hpp
    utils/lru_cache.hpp
    utils/rating_normalizer.cpp
    utils/rating_normalizer.hpp
    ipc/ipc_manager.cpp
    ipc/ipc_manager.hpp
    ipc/ipc_types.hpp
    events/event_system.hpp
    config/config_manager.hpp
    config/config_manager.cpp
)

target_include_directories(core
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(core
    PUBLIC
        services
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)