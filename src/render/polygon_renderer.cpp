#include "render/polygon_renderer.h"

#include <iostream>
#include <stack>

PolygonVertex::PolygonVertex(glm::vec3 position, glm::vec4 color)
{
    // Position straightforward
    this->position = position;
    this->color = color;
}

void PolygonRenderer::initialise(unsigned int program_id)
{
    this->program_id = program_id;
    m_loc = glGetUniformLocation(program_id, "M");
    v_loc = glGetUniformLocation(program_id, "V");
}

void PolygonRenderer::enable(const glm::mat4 &view)
{
    glUseProgram(program_id);
    glBindVertexArray(static_VAO);
    glUniformMatrix4fv(v_loc, 1, GL_FALSE, &view[0][0]);
}

void PolygonRenderer::update_polygon(component::Polygon &polygon)
{
    if (!polygon.allocated) {
        polygon.vertices_offset = static_vertices.size();
        polygon.indices_offset = static_indices.size();
        static_vertices.resize(static_vertices.size() + polygon.vertices.size());
        static_indices.resize(static_indices.size() + polygon.indices.size());
        for (std::size_t i = 0; i < polygon.vertices.size(); i++) {
            static_vertices[i].position.z = 0;
        }
    }

    if (polygon.dirty) {
        for (std::size_t i = 0; i < polygon.vertices.size(); i++) {
            static_vertices[i].position.x = polygon.vertices[i].x;
            static_vertices[i].position.y = polygon.vertices[i].y;
            static_vertices[i].color = polygon.colors[i];
        }
        polygon.dirty = false;
    }
}

void PolygonRenderer::reinitialise()
{
    // 1. Generate ids

    glGenVertexArrays(1, &static_VAO);
    glGenBuffers(1, &static_VBO);
    glGenBuffers(1, &static_EBO);

    // 2. Bind vertex array
    glBindVertexArray(static_VAO);

    // 3. Bind and configure buffer for vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, static_VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_vertices.size() * sizeof(PolygonVertex),
        &static_vertices[0],
        GL_STATIC_DRAW
    );

    // 4. Bind and configure buffer for vertex attributes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_indices.size() * sizeof(unsigned short),
        &static_indices[0],
        GL_STATIC_DRAW
    );

    // 5. Define the vertex buffer attributes

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(PolygonVertex),
        (void*)0
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1, 4, GL_FLOAT, GL_TRUE, sizeof(PolygonVertex),
        (void*)offsetof(PolygonVertex, color)
    );
    glEnableVertexAttribArray(1);

}

void PolygonRenderer::render(const component::Polygon &polygon)
{
    glUniformMatrix4fv(m_loc, 1, GL_FALSE, &polygon.model[0][0]);

    glDrawElementsBaseVertex(
        GL_TRIANGLES, polygon.element_count, GL_UNSIGNED_SHORT,
        (void*)(sizeof(unsigned short) * polygon.indices_offset),
        polygon.vertices_offset
    );
}

