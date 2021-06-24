#ifndef RENDER_SPRITE_RENDERER_H
#define RENDER_SPRITE_RENDERER_H

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "render/textures.h"

// Passed to renderer to configure sprite
struct SpriteConfig {
    glm::vec<2, GLint> pos, size, offset;
    Texture diffuse_texture;
};

// Stored information for sprite
struct Sprite {
    unsigned int diffuse_texture_id;
    unsigned int vertices_offset;
    unsigned int indices_offset;
};

struct SpriteVertex {
    // 2D x, y, but add z for controlling draw order
    glm::vec<3, GLfloat> position;
    glm::vec<2, GLushort> tex_coords;
    SpriteVertex(glm::vec3 position, glm::vec2 tex_coords);
};


class SpriteRenderer {
public:
    SpriteRenderer() {}

    void load_sprite(const SpriteConfig &config);
    void initialise(unsigned int program_id);

    struct Params {
        const glm::mat4 *view;
    };
    void enable(const Params &params);

    struct Command {
        const glm::mat4 *model;
        std::size_t sprite_index;
    };
    void render(const Command &command);

private:
    // OpenGL data
    std::vector<SpriteVertex> static_vertices;
    std::vector<unsigned short> static_indices;
    unsigned int static_VAO, static_VBO, static_EBO;

    // OpenGL shader parameter locations
    unsigned int program_id;
    unsigned int v_loc;
    unsigned int m_loc;
    unsigned int diffuse_texture_loc;

    std::vector<Sprite> sprites;
};

#endif
