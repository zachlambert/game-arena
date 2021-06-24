#include "render/mesh_renderer.h"

#include <stack>

MeshVertex::MeshVertex(glm::vec3 position, glm::vec4 color)
{
    // Position straightforward
    this->position = position;
    this->color = color;
}

// For use in the triangulate_mesh function
struct MeshComponent {
    glm::vec2 vertex;
    glm::vec4 color;
};

bool vertex_on_the_left(glm::vec2 first, glm::vec2 second, glm::vec2 query)
{
    glm::vec2 dif = second - first;
    glm::vec3 perp = glm::cross(glm::vec3(0, 0, 1), glm::vec3(dif.x, dif.y, 0));
    // Perp is to the left of the first displacmenet
    // A positive component along this will indicate the query point is to the left
    dif = query - second;
    double component = glm::dot(perp, glm::vec3(dif.x, dif.y, 0));
    return (component > 0);
}

bool triangle_valid(int a, int b, int c, const std::vector<MeshComponent> mesh)
{
    // 1. Triangle is clockwise
    if (vertex_on_the_left(mesh[a].vertex, mesh[b].vertex, mesh[c].vertex)) return false;

    // 2. No self-intersection
    for (int q = b+1; q < c; q++) {
        if (vertex_on_the_left(mesh[a].vertex, mesh[b].vertex, mesh[q].vertex)) continue;
        if (vertex_on_the_left(mesh[b].vertex, mesh[c].vertex, mesh[q].vertex)) continue;
        if (vertex_on_the_left(mesh[c].vertex, mesh[a].vertex, mesh[q].vertex)) continue;
        return false;
    }

    return true;
}

MeshConfig triangulate_mesh(std::vector<glm::vec2> vertices, glm::vec4 color)
{
    // Assumes the vertices to be provided in a clockwise order.
    // Works on any mesh
    MeshConfig mesh;

    std::vector<MeshComponent> initial(vertices.size());
    for (int i = 0; i < vertices.size(); i++) {
        initial[i].vertex = vertices[i];
        initial[i].color = color;
    }

    std::stack<std::vector<MeshComponent>> stack;
    stack.push(initial);

    int a, b, c;
    while (!stack.empty()) {
        const std::vector<MeshComponent> &current = stack.top();
        a = 0;
        b = 1;
        c = 2;
        bool valid = false;
        while (c!= current.size()) {
            bool current_valid = triangle_valid(a, b, c, current);
            if (current_valid) {
                mesh.vertices.push_back(current[a].vertex);
                mesh.vertices.push_back(current[b].vertex);
                mesh.vertices.push_back(current[c].vertex);
                mesh.colors.push_back(current[a].color);
                mesh.colors.push_back(current[b].color);
                mesh.colors.push_back(current[c].color);
                mesh.indices.push_back(vertices.size()-3);
                mesh.indices.push_back(vertices.size()-2);
                mesh.indices.push_back(vertices.size()-1);

                if (!valid) {
                    valid = true;
                    int new_start = b;
                    int new_end = c;
                    std::vector<MeshComponent> new_mesh(vertices.size());
                    for (int i = 0; i < c-b+1; i++) {
                        new_mesh[i] = current[b+i];
                    }
                    stack.push(new_mesh);
                }
                b = c;
            } else {
                valid = false;
            }
            c++;
        }
    }

    return mesh;
}

void MeshRenderer::load_mesh(const MeshConfig &config)
{
    Mesh mesh;

    mesh.vertices_offset = static_vertices.size();
    for (int i = 0; i < config.vertices.size(); i++) {
        static_vertices.push_back(MeshVertex(
            glm::vec3(config.vertices[i].x, config.vertices[i].y, 0),
            config.colors[i]
        ));
    }

    mesh.indices_offset = static_indices.size();
    std::copy(config.indices.begin(), config.indices.end(), static_indices.begin());

    mesh.element_count = config.indices.size();

    meshes.push_back(mesh);
}

void MeshRenderer::initialise(unsigned int program_id)
{
    this->program_id = program_id;
    m_loc = glGetUniformLocation(program_id, "M");
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
        static_vertices.size() * sizeof(MeshVertex),
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
        0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex),
        (void*)0
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1, 4, GL_FLOAT, GL_TRUE, sizeof(MeshVertex),
        (void*)offsetof(MeshVertex, color)
    );
    glEnableVertexAttribArray(1);
}

void MeshRenderer::enable(const Params &params)
{
    glUseProgram(program_id);
    glBindVertexArray(static_VAO);
    glUniformMatrix4fv(v_loc, 1, GL_FALSE, &(*params.view)[0][0]);
}

void MeshRenderer::render(const Command &command)
{
    const Mesh &mesh = meshes[command.mesh_index];

    glUniformMatrix4fv(m_loc, 1, GL_FALSE, &(*command.model)[0][0]);

    glDrawElementsBaseVertex(
        GL_TRIANGLES, mesh.element_count, GL_UNSIGNED_SHORT,
        (void*)(sizeof(unsigned short) * mesh.indices_offset),
        mesh.vertices_offset
    );
}

