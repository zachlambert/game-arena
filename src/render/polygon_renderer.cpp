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

    // Force a reinitialisation of vertices and indices
    vertices.resize_flag = true;
    indices.resize_flag = true;
    reinitialise();
}

void PolygonRenderer::enable(const glm::mat4 &view)
{
    glUseProgram(program_id);
    glBindVertexArray(static_VAO);
    glUniformMatrix4fv(v_loc, 1, GL_FALSE, &view[0][0]);
}

void PolygonRenderer::store_polygon(const component::Polygon &polygon)
{
    if (polygon.to_deallocate) {
        vertices.deallocate(polygon.vertices.size(), polygon.vertices_offset);
        indices.deallocate(polygon.indices.size(), polygon.indices_offset);
    }

    if (!polygon.allocated) {
        std::cout << "Allocating" << std::endl;
        std::cout << "Num vertices = " << polygon.vertices.size() << std::endl;
        polygon.allocated = true;
        // TODO: !!! Will break of vertices and indices sizes aren't multiples of 2
        vertices.allocate(polygon.vertices.size(), polygon.vertices_offset);
        indices.allocate(polygon.indices.size(), polygon.indices_offset);
        for (std::size_t i = 0; i < polygon.vertices.size(); i++) {
            vertices.data[i].position.z = 0;
        }
        std::cout << "Allocated" << std::endl;
    }

    if (polygon.dirty) {
        for (std::size_t i = 0; i < polygon.vertices.size(); i++) {
            vertices.data[polygon.vertices_offset+i].position.x = polygon.vertices[i].x;
            vertices.data[polygon.vertices_offset+i].position.y = polygon.vertices[i].y;
            vertices.data[polygon.vertices_offset+i].color = polygon.colors[i];
        }
        for (std::size_t i = 0; i < polygon.indices.size(); i++) {
            indices.data[polygon.indices_offset+i] = polygon.indices[i];
        }
        polygon.dirty = false;

        if (!vertices.resize_flag) {
            glBindVertexArray(static_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, static_VBO);
            glBufferSubData(
                GL_ARRAY_BUFFER,
                polygon.vertices_offset*sizeof(PolygonVertex),
                polygon.vertices.size()*sizeof(PolygonVertex),
                &vertices.data[polygon.vertices_offset]
            );
        }
        // TODO: Use seperate flag for when indices change
        if (!indices.resize_flag) {
            glBindVertexArray(static_VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_EBO);
            glBufferSubData(
                GL_ELEMENT_ARRAY_BUFFER,
                polygon.indices_offset*sizeof(unsigned short),
                polygon.indices.size()*sizeof(unsigned short),
                &indices.data[polygon.indices_offset]
            );
        }
    }
}

void PolygonRenderer::reinitialise()
{
    glBindVertexArray(static_VAO);

    if (vertices.resize_flag) {
        vertices.resize_flag = false;
        glBindBuffer(GL_ARRAY_BUFFER, static_VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertices.data.size() * sizeof(PolygonVertex),
            &vertices.data[0],
            GL_STREAM_DRAW
        );
    }

    if (indices.resize_flag) {
        indices.resize_flag = false;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_EBO);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indices.data.size() * sizeof(unsigned short),
            &indices.data[0],
            GL_STREAM_DRAW
        );
    }
}

void PolygonRenderer::render(const component::Polygon &polygon)
{
    if (polygon.to_deallocate) return;

    glUniformMatrix4fv(m_loc, 1, GL_FALSE, &polygon.model[0][0]);

    glDrawElementsBaseVertex(
        GL_TRIANGLES, polygon.element_count, GL_UNSIGNED_SHORT,
        (void*)(sizeof(unsigned short) * polygon.indices_offset),
        polygon.vertices_offset
    );
}

void PolygonRenderer::deallocate_polygon(const component::Polygon &polygon)
{
    vertices.deallocate(polygon.vertices.size(), polygon.vertices_offset);
    indices.deallocate(polygon.indices.size(), polygon.indices_offset);
}
