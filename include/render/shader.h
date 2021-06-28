#ifndef MIST_RENDER_SHADERS_MANAGER_H
#define MIST_RENDER_SHADERS_MANAGER_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

unsigned int load_shader(const std::string &vs_path, const std::string &fs_path);

#endif
