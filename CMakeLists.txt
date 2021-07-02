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

    src/game/animation.cpp
    src/game/camera.cpp
    src/game/collision.cpp
    src/game/entity.cpp
    src/game/game.cpp

    src/game/system/collision.cpp
    src/game/system/collision_physics.cpp
    src/game/system/enemy.cpp
    src/game/system/enemy_spawner.cpp
    src/game/system/gun.cpp
    src/game/system/occlusion_polygon.cpp
    src/game/system/physics.cpp
    src/game/system/player.cpp
    src/game/system/render_sprite.cpp
    src/game/system/render_polygon.cpp

    src/render/collision_renderer.cpp
    src/render/polygon_renderer.cpp
    src/render/renderer.cpp
    src/render/shader.cpp
    src/render/sprite_renderer.cpp
    src/render/terrain_renderer.cpp
    src/render/texture.cpp

    src/setup/entity_builder.cpp
    src/setup/world_builder.cpp
    src/setup/sprite_config.cpp

    src/window/clock.cpp
    src/window/fps_counter.cpp
    src/window/input.cpp
    src/window/window.cpp
)

add_executable(game ${SRC})

target_include_directories(game
    PRIVATE include
)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE /opt/stb)

target_link_libraries(game PRIVATE GL glfw GLEW assimp stb)


# Add some tests

add_executable(test_heap test/heap.cpp)
target_include_directories(test_heap PRIVATE include)
