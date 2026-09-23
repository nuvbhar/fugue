cmake_minimum_required(VERSION 3.28)

project(fugue
    VERSION 0.1.0
    DESCRIPTION "A modular TUI music player"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(Platform)

# Third-party submodules
# json
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(third_party/json)

# miniaudio (header-only, create interface target)
add_library(miniaudio INTERFACE)
target_include_directories(miniaudio INTERFACE ${CMAKE_SOURCE_DIR}/third_party/miniaudio)

# Core executable
add_executable(fugue src/main.cpp)

# Apply warnings and platform defines
set_project_warnings(fugue)
# Platform defines are handled inside Platform.cmake

target_link_libraries(fugue PRIVATE nlohmann_json::nlohmann_json miniaudio)
