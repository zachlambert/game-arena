#ifndef RENDER_MESH_RENDERER_H
#define RENDER_MESH_RENDERER_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct MeshConfig {
    std::vector<glm::vec2> vertices;
    std::vector<glm::vec4> colors;
    std::vector<unsigned int> indices;
};

MeshConfig triangulate_mesh(std::vector<glm::vec2> vertices, glm::vec4 color);

struct MeshVertex {
    glm::vec<3, GLfloat> position;
    glm::vec<4, GLfloat> color; // with opacity
    MeshVertex(glm::vec3 position, glm::vec4 color);
};

struct Mesh {
    unsigned int vertices_offset;
    unsigned int indices_offset;
    unsigned int element_count;
};

class MeshRenderer {
public:
    MeshRenderer() {}

    void load_mesh(const MeshConfig &config);
    void initialise(unsigned int program_id);

    struct Params {
        const glm::mat4 *view;
    };
    void enable(const Params &params);

    struct Command {
        const glm::mat4 *model;
        std::size_t mesh_index;
    };
    void render(const Command &command);

private:
    // OpenGL data
    std::vector<MeshVertex> static_vertices;
    std::vector<unsigned short> static_indices;
    unsigned int static_VAO, static_VBO, static_EBO;

    // OpenGL shader parameter locations
    unsigned int program_id;
    unsigned int v_loc;
    unsigned int m_loc;

    std::vector<Mesh> meshes;
};

#endif
