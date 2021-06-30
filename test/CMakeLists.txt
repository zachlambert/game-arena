add_executable(test_heap )

target_include_directories(game
    PRIVATE include
)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE /opt/stb)

target_link_libraries(game PRIVATE GL glfw GLEW assimp stb)
