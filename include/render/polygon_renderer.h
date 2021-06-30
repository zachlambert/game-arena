#ifndef RENDER_MESH_RENDERER_H
#define RENDER_MESH_RENDERER_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "data_structures/heap.h"
#include "game/components.h"

struct PolygonVertex {
    glm::vec<3, GLfloat> position;
    glm::vec<4, GLfloat> color; // with opacity
    PolygonVertex(): position(0, 0, 0), color(0, 0, 0, 0) {}
    PolygonVertex(glm::vec3 position, glm::vec4 color);
};

class PolygonRenderer {
public:
    PolygonRenderer() {}

    void initialise(unsigned int program_id);

    void enable(const glm::mat4 &view);
    void store_polygon(const component::Polygon &polygon);
    void reinitialise();
    void render(const component::Polygon &polygon);

    void deallocate_polygon(const component::Polygon &polygon);

private:

    // OpenGL data
    Heap<PolygonVertex> vertices;
    Heap<unsigned short> indices;
    unsigned int static_VAO, static_VBO, static_EBO;

    // OpenGL shader parameter locations
    unsigned int program_id;
    unsigned int v_loc;
    unsigned int m_loc;
};

#endif
