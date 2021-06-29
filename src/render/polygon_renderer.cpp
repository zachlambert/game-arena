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
    dirty = true;

    glGenVertexArrays(1, &static_VAO);
    glGenBuffers(1, &static_VBO);
    glGenBuffers(1, &static_EBO);

    glBindVertexArray(static_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, static_VBO);

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

void PolygonRenderer::enable(const glm::mat4 &view)
{
    glUseProgram(program_id);
    glBindVertexArray(static_VAO);
    glUniformMatrix4fv(v_loc, 1, GL_FALSE, &view[0][0]);
}

void PolygonRenderer::store_polygon(const component::Polygon &polygon)
{
    if (!polygon.allocated) {
        polygon.allocated = true;
        polygon.vertices_offset = static_vertices.size();
        polygon.indices_offset = static_indices.size();
        static_vertices.resize(static_vertices.size() + polygon.vertices.size());
        static_indices.resize(static_indices.size() + polygon.indices.size());
        for (std::size_t i = 0; i < polygon.vertices.size(); i++) {
            static_vertices[i].position.z = 0;
        }
        dirty = true;
    }

    if (polygon.dirty) {
        for (std::size_t i = 0; i < polygon.vertices.size(); i++) {
            static_vertices[polygon.vertices_offset+i].position.x = polygon.vertices[i].x;
            static_vertices[polygon.vertices_offset+i].position.y = polygon.vertices[i].y;
            static_vertices[polygon.vertices_offset+i].color = polygon.colors[i];
        }
        for (std::size_t i = 0; i < polygon.indices.size(); i++) {
            static_indices[polygon.indices_offset+i] = polygon.indices[i];
        }
        polygon.dirty = false;

        if (!dirty) {
            glBindVertexArray(static_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, static_VBO);
            glBufferSubData(
                GL_ARRAY_BUFFER,
                polygon.vertices_offset*sizeof(PolygonVertex),
                polygon.vertices.size()*sizeof(PolygonVertex),
                &static_vertices[polygon.vertices_offset]
            );
            // TODO: Have seperate flag for when indices have changed, since
            // this is more rare.
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_EBO);
            glBufferSubData(
                GL_ELEMENT_ARRAY_BUFFER,
                polygon.indices_offset*sizeof(PolygonVertex),
                polygon.indices.size()*sizeof(PolygonVertex),
                &static_indices[polygon.vertices_offset]
            );
        }
    }
}

void PolygonRenderer::reinitialise()
{
    if (!dirty) return;
    dirty = false;

    glBindVertexArray(static_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, static_VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_vertices.size() * sizeof(PolygonVertex),
        &static_vertices[0],
        GL_STREAM_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_indices.size() * sizeof(unsigned short),
        &static_indices[0],
        GL_STREAM_DRAW
    );
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

