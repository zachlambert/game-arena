#ifndef RENDER_SPRITE_MANAGER_H
#define RENDER_SPRITE_MANAGER_H

#include <unordered_map>
#include <string>

struct Texture {
    unsigned int id;
    int width, height, num_channels;
};

class TextureManager{
public:
    TextureManager(std::string base_dir):base_dir(base_dir + "textures/") {}
    bool load_texture(const std::string &name, const std::string &relative_path);
    Texture get_texture(const std::string &name)const{ return textures.at(name); }
private:
    std::string base_dir;
    std::unordered_map<std::string, Texture> textures;
};

#endif
