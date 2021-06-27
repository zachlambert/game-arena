#ifndef SETUP_SPRITE_CONFIG_H
#define SETUP_SPRITE_CONFIG_H

#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "sprite_id.h"

struct SpriteConfig {
    SpriteId id;
    glm::vec<2, GLint> pos, size, offset;
    std::vector<glm::vec2> collision_polygon;
};

struct SpritesheetConfig {
    std::string diffuse_texture_file_name;
    std::vector<SpriteConfig> sprites;
};

void load_spritesheets(std::vector<SpritesheetConfig> spritesheets);

#endif
