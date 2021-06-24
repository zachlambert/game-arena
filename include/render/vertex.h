#ifndef RENDER_VERTEX_H
#define RENDER_VERTEX_H

#include <GL/glew.h>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec<3, GLfloat> position; // 2D x, y, but add z for controlling draw order
    glm::vec<2, GLushort> tex_coords;
    Vertex(glm::vec3 position, glm::vec2 tex_coords);
};

#endif
