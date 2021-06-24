cmake_minimum_required(VERSION 3.0)
project(
    "Arena shooter"
    VERSION 0.0.1
    DESCRIPTION "Top-down arena shooter."
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(SRC
    src/main.cpp

    src/window/clock.cpp
    src/window/window.cpp
    src/window/input.cpp

    src/render/renderer.cpp
    src/render/shaders.cpp
    src/render/sprite_renderer.cpp
    src/render/textures.cpp

    src/world/world.cpp
)

add_executable(game ${SRC})

target_include_directories(game
    PRIVATE include
)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE /opt/stb)

target_link_libraries(game PRIVATE GL glfw GLEW assimp stb)
