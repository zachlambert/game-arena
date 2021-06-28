#ifndef MIST_RENDER_RENDERER_H
#define MIST_RENDER_RENDERER_H

#include <list>
#include <set>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "setup/resources.h"
#include "render/shader.h"
#include "render/sprite_renderer.h"
// #include "render/mesh_renderer.h"
#include "render/terrain_renderer.h"
#include "game/game.h"

class Renderer {
public:
    Renderer(const std::string &base_dir): base_dir(base_dir) {}

    void initialise(const Resources &resources, const Game &game);
    void render(const Game &game);

    SpriteRenderer sprite_renderer;
    // MeshRenderer mesh_renderer;
    TerrainRenderer terrain_renderer;

private:
    std::string base_dir;
};

#endif
