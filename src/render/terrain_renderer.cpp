#include "render/terrain_renderer.h"

#include <iostream>
#include <stack>

TerrainVertex::TerrainVertex(glm::vec2 position, glm::vec4 color, int depth)
{
    // Position straightforward
    this->position = glm::vec3(position.x, position.y, (float)depth);
    this->color = color;
}

// ------ MESH TRIANGULATION -----

struct MeshComponent {
    glm::vec2 vertex;
    glm::vec4 color;
};

static bool vertex_on_the_left(glm::vec2 first, glm::vec2 second, glm::vec2 query)
{
    glm::vec2 dif = second - first;
    glm::vec3 perp = glm::cross(glm::vec3(0, 0, 1), glm::vec3(dif.x, dif.y, 0));
    // Perp is to the left of the first displacmenet
    // A positive component along this will indicate the query point is to the left
    dif = query - second;
    double component = glm::dot(perp, glm::vec3(dif.x, dif.y, 0));
    return (component > 0);
}

static bool triangle_valid(unsigned short i, unsigned short j, unsigned short k, const std::vector<glm::vec2> vertices)
{
    // 1. Triangle is clockwise
    if (vertex_on_the_left(vertices[i], vertices[j], vertices[k])) return false;

    // 2. No self-intersection
    for (unsigned short q = j+1; q < k; q++) {
        if (vertex_on_the_left(vertices[i], vertices[j], vertices[q])) continue;
        if (vertex_on_the_left(vertices[j], vertices[k], vertices[q])) continue;
        if (vertex_on_the_left(vertices[k], vertices[i], vertices[q])) continue;
        return false;
    }

    return true;
}

void TerrainRenderer::load_terrain_element(const TerrainElement &element)
{
    // Assumes the vertices to be provided in a clockwise order.
    // Works on any mesh

    int base_vertex_index = static_vertices.size();
    for (int i = 0; i < element.vertices.size(); i++) {
        static_vertices.push_back(TerrainVertex(
            element.pos + element.vertices[i],
            element.color,
            element.visual_depth
        ));
    }

    std::vector<unsigned short> initial(element.vertices.size());
    for (unsigned short i = 0; i < element.vertices.size(); i++) {
        initial[i] = i;
    }

    std::stack<std::vector<unsigned short>> stack;
    stack.push(initial);

    int a, b, c;
    while (!stack.empty()) {
        const std::vector<unsigned short> current = stack.top();
        stack.pop();
        a = 0;
        b = 1;
        c = 2;
        bool valid = true;
        while (c!= current.size()) {
            bool current_valid = triangle_valid(current[a], current[b], current[c], element.vertices);
            if (current_valid) {
                static_indices.push_back(base_vertex_index + current[a]);
                static_indices.push_back(base_vertex_index + current[b]);
                static_indices.push_back(base_vertex_index + current[c]);

                if (!valid) {
                    valid = true;
                    int new_start = b;
                    int new_end = c;
                    std::vector<unsigned short> new_indices(c-b+1);
                    for (int i = 0; i < c-b+1; i++) {
                        new_indices[i] = current[b+i];
                    }
                    stack.push(new_indices);
                }
                b = c;
            } else {
                valid = false;
            }
            c++;
        }
        if (!valid) {
            int new_start = b;
            int new_end = c-1;
            std::vector<unsigned short> new_indices(c-b+1);
            for (int i = 0; i < c-b+1; i++) {
                new_indices[i] = current[b+i];
            }
            stack.push(new_indices);
        }
    }
}

void TerrainRenderer::load_terrain(const Terrain &terrain)
{
    for (const auto &element: terrain.elements) {
        load_terrain_element(element);
    }
}

void TerrainRenderer::initialise(unsigned int program_id)
{
    this->program_id = program_id;
    v_loc = glGetUniformLocation(program_id, "V");

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
        static_vertices.size() * sizeof(TerrainVertex),
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
        0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex),
        (void*)0
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1, 4, GL_FLOAT, GL_TRUE, sizeof(TerrainVertex),
        (void*)offsetof(TerrainVertex, color)
    );
    glEnableVertexAttribArray(1);
}

void TerrainRenderer::enable(const Params &params)
{
    glUseProgram(program_id);
    glBindVertexArray(static_VAO);
    glUniformMatrix4fv(v_loc, 1, GL_FALSE, &(*params.view)[0][0]);
}

void TerrainRenderer::render()
{
    glDrawElements(
        GL_TRIANGLES, static_indices.size(), GL_UNSIGNED_SHORT, 0
    );
}

