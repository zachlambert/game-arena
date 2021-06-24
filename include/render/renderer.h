#ifndef MIST_RENDER_RENDERER_H
#define MIST_RENDER_RENDERER_H

#include <list>
#include <set>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "render/textures.h"
#include "render/shaders.h"
#include "render/sprite_renderer.h"
#include "render/mesh_renderer.h"

class World;

class Renderer {
public:
    Renderer(const std::string &base_dir):
        texture_manager(base_dir),
        shaders(base_dir)
    {}

    void initialise();
    void render(const World &world);
private:
    TextureManager texture_manager;
    Shaders shaders;
    SpriteRenderer sprite_renderer;
    MeshRenderer mesh_renderer;
};

#endif
