#ifndef MIST_RENDER_SHADERS_MANAGER_H
#define MIST_RENDER_SHADERS_MANAGER_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct Shaders {
    unsigned int sprite_program_id;
    unsigned int mesh_program_id;
    unsigned int terrain_program_id;
    Shaders(const std::string &base_dir);
};

#endif
