#ifndef RENDER_SPRITE_MANAGER_H
#define RENDER_SPRITE_MANAGER_H

#include <unordered_map>
#include <string>

struct Texture {
    unsigned int id;
    int width, height, num_channels;
};

Texture load_texture(const std::string &texture_path);

#endif
