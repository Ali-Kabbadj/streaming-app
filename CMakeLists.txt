cmake_minimum_required(VERSION 3.20)
project(streaming_app VERSION 0.1.0 LANGUAGES CXX)
cmake_policy(SET CMP0011 NEW)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable folders for IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Configure compilation
if(MSVC)
    add_compile_options(/W4 /WX)
    add_definitions(-D_WIN32_WINNT=0x0A00) # Target Windows 10
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# Find dependencies
find_package(unofficial-webview2 CONFIG REQUIRED)
find_package(WIL CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add subdirectories
add_subdirectory(src)

# Enable testing
enable_testing()