#ifndef GAME_TERRAIN_H
#define GAME_TERRAIN_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct TerrainElement {
    glm::vec2 pos;
    std::vector<glm::vec2> vertices;
    glm::vec4 color;
    int visual_depth;
};

struct Terrain {
    std::vector<TerrainElement> elements;
    glm::vec2 centre;
    glm::vec2 size;
};

#endif
