cmake_minimum_required(VERSION 3.20)
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)


# Set vcpkg toolchain before project() call
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()


# Enable Hot Reload for MSVC
if(POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()



project(streaming_app VERSION 0.1.0 LANGUAGES CXX)
cmake_policy(SET CMP0011 NEW)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set warrning policy
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX-")



# Enable folders for IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Configure compilation
if(MSVC)
    add_compile_options(/EHsc /W4)

    add_definitions(-D_WIN32_WINNT=0x0A00) # Target Windows 10
      add_compile_options(/EHsc)
    # Optional: To suppress specific warnings
    add_compile_options(/wd4530)
    add_compile_options(/wd4251)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

if(CMAKE_VERSION VERSION_GREATER "3.15")
    cmake_policy(SET CMP0091 NEW)  # Enable new MSVC runtime library flag handling
endif()

# If you still see the warning, you can explicitly set the runtime library
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Set vcpkg install directory to be inside build
set(VCPKG_INSTALLED_DIR ${CMAKE_BINARY_DIR}/vcpkg_installed)

# Find dependencies


find_package(nlohmann_json CONFIG REQUIRED)
find_package(curl CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)
find_package(unofficial-webview2 CONFIG REQUIRED)
find_package(WIL CONFIG REQUIRED)
find_package(date CONFIG REQUIRED)

# Add subdirectories
add_subdirectory(src)

# Enable testing
enable_testing()