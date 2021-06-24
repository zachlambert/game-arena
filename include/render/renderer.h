#ifndef MIST_RENDER_RENDERER_H
#define MIST_RENDER_RENDERER_H

#include <list>
#include <set>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "render/shaders.h"
#include "render/textures.h"

struct Vertex {
    glm::vec<3, GLfloat> position; // 2D x, y, but add z for controlling draw order
    glm::vec<2, GLushort> tex_coords;
    Vertex(glm::vec3 position, glm::vec2 tex_coords);
};


class Mesh {
public:
    Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned short> &indices): vertices(vertices), indices(indices) {}

    // Raw mesh data
    std::vector<Vertex> vertices;
    std::vector<unsigned short> indices;
    std::size_t vertices_offset;
    std::size_t indices_offset;
    std::size_t index;
};

struct MeshMinimal {
    std::size_t index_count;
    std::size_t indices_offset;
    std::size_t vertices_offset;
};

// Passed to renderer to configure sprite
struct SpriteConfig {
    glm::vec<2, GLushort> pos, size;
    glm::vec<2, GLshort> offset;
    unsigned int texture_id;
};

// Stored information for sprite
struct Sprite {
    unsigned int diffuse_texture_id;
    unsigned int vertices_offset;
    unsigned int indices_offset;
};

class SpriteRenderer {
public:
    SpriteRenderer() {}

    void load_sprite(const SpriteConfig &config);
    void initialise(unsigned int program_id);

    struct Params {
        const glm::mat4 &view;
    };
    void enable(const Params &params);

    struct Command {
        const glm::mat4 &model;
        std::size_t sprite_index;
    };
    void render(const Command &command);

private:
    // OpenGL data
    std::vector<Vertex> static_vertices;
    std::vector<unsigned short> static_indices;
    unsigned int static_VAO, static_VBO, static_EBO;

    // OpenGL shader parameter locations
    unsigned int program_id;
    unsigned int v_loc;
    unsigned int m_loc;
    unsigned int diffuse_texture_loc;

    std::vector<Sprite> sprites;
};

class Renderer {
public:
    Renderer(const std::string &base_dir = "data/");
    void initialise();
    void render();
private:
    TextureManager texture_manager;
    Shaders shaders;
    SpriteRenderer sprite_renderer;
};

#endif
