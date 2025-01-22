add_library(core
    utils/logger.cpp
    utils/logger.hpp
    utils/result.hpp
    utils/win32_utils.hpp
    utils/win32_utils.cpp
    ipc/ipc_manager.cpp
    ipc/ipc_manager.hpp
)

target_include_directories(core
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(core
    PUBLIC
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)