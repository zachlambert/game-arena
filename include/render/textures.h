#ifndef RENDER_SPRITE_MANAGER_H
#define RENDER_SPRITE_MANAGER_H

#include <unordered_map>
#include <string>

class TextureManager{
public:
    TextureManager(std::string base_dir):base_dir(base_dir + "textures/") {}
    unsigned int get_texture_id(const std::string &relative_path);
private:
    std::string base_dir;
    std::unordered_map<std::string, unsigned int> textures;
};

#endif
