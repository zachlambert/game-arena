#ifndef RENDER_TERRAIN_RENDERER_H
#define RENDER_TERRAIN_RENDERER_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct TerrainElement {
    glm::vec2 pos;
    std::vector<glm::vec2> vertices;
    glm::vec4 color; // For now, just a single colour
    int depth;
};

struct TerrainVertex {
    glm::vec<3, GLfloat> position;
    glm::vec<4, GLfloat> color; // with opacity
    TerrainVertex(glm::vec2 position, glm::vec4 color, int depth);
};

class TerrainRenderer {
public:
    TerrainRenderer() {}

    void add_terrain_element(const TerrainElement &element);
    void initialise(unsigned int program_id);

    struct Params {
        const glm::mat4 *view;
    };
    void enable(const Params &params);

    void render();

private:
    // OpenGL data
    std::vector<TerrainVertex> static_vertices;
    std::vector<unsigned short> static_indices;
    unsigned int static_VAO, static_VBO, static_EBO;

    // OpenGL shader parameter locations
    unsigned int program_id;
    unsigned int v_loc;
};

#endif
