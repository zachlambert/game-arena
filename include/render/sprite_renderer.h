#ifndef RENDER_SPRITE_RENDERER_H
#define RENDER_SPRITE_RENDERER_H

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "setup/sprite_config.h"
#include "render/texture.h"

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

    void load_spritesheets(const std::string &base_dir, const std::vector<SpritesheetConfig> spritesheets);
    void initialise(unsigned int program_id);

    struct Params {
        const glm::mat4 *view;
    };
    void enable(const Params &params);

    struct Command {
        const glm::mat4 *model;
        SpriteId sprite_id;
    };
    void render(const Command &command);

private:
    void load_sprite(const SpriteConfig &sprite, const Texture &diffuse_texture);
    std::unordered_map<std::string, int> sprite_indices;

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
