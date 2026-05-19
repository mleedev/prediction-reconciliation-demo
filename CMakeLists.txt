cmake_minimum_required(VERSION 3.20)
project(prediction-reconciliation-demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Raylib
set(RAYLIB_VERSION 6.0)
include(FetchContent)
FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(raylib)

# Server executable
add_executable(server src/server/main.cpp)
target_include_directories(server PRIVATE src)

# Client executable
add_executable(client src/client/main.cpp)
target_include_directories(client PRIVATE src)
target_link_libraries(client PRIVATE raylib)

# Required frameworks for raylib if on MacOS
if (APPLE)
    target_link_libraries(client PRIVATE "-framework IOKit")
    target_link_libraries(client PRIVATE "-framework Cocoa")
    target_link_libraries(client PRIVATE "-framework OpenGL")
endif()

# Winsock on Windows
if(WIN32)
    target_link_libraries(server PRIVATE ws2_32)
    target_link_libraries(client PRIVATE ws2_32)
endif()
