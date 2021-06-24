#include "render/sprite_renderer.h"
#include <iostream>

void SpriteRenderer::load_sprite(const SpriteConfig &config)
{
    double left = -config.size[0]/2 - config.offset[0];
    double top = config.size[1]/2 - config.offset[1];
    double right = config.size[0]/2 - config.offset[0];
    double bot = -config.size[1]/2 - config.offset[1];

    double uv_left = (double)config.pos[0]/config.diffuse_texture.width;
    double uv_top = (double)(config.pos[1])/config.diffuse_texture.height;
    double uv_right = (double)(config.pos[0]+config.size[0])/config.diffuse_texture.width;
    double uv_bot = (double)(config.pos[1] + config.size[1])/config.diffuse_texture.height;

    unsigned int vertices_offset = static_vertices.size();

    static_vertices.push_back(Vertex(
        glm::vec3(left, top, 0),
        glm::vec2(uv_left, uv_top)
    ));
    static_vertices.push_back(Vertex(
        glm::vec3(right, top, 0),
        glm::vec2(uv_right, uv_top)
    ));
    static_vertices.push_back(Vertex(
        glm::vec3(right, bot, 0),
        glm::vec2(uv_right, uv_bot)
    ));
    static_vertices.push_back(Vertex(
        glm::vec3(left, bot, 0),
        glm::vec2(uv_left, uv_bot)
    ));

    unsigned int indices_offset = static_indices.size();

    static_indices.push_back(0);
    static_indices.push_back(1);
    static_indices.push_back(2);
    static_indices.push_back(0);
    static_indices.push_back(2);
    static_indices.push_back(3);

    Sprite sprite;
    sprite.diffuse_texture_id = config.diffuse_texture.id;
    sprite.vertices_offset = vertices_offset;
    sprite.indices_offset = indices_offset;
    sprites.push_back(sprite);
}

void SpriteRenderer::initialise(unsigned int program_id)
{
    this->program_id = program_id;
    m_loc = glGetUniformLocation(program_id, "M");
    v_loc = glGetUniformLocation(program_id, "V");
    diffuse_texture_loc = glGetUniformLocation(program_id, "diffuse_texture");

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
        static_vertices.size() * sizeof(Vertex),
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
        0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)0
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(Vertex),
        (void*)offsetof(Vertex, tex_coords)
    );
    glEnableVertexAttribArray(1);
}

void SpriteRenderer::enable(const Params &params)
{
    glUseProgram(program_id);
    glBindVertexArray(static_VAO);
    glUniformMatrix4fv(v_loc, 1, GL_FALSE, &(*params.view)[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(diffuse_texture_loc, 0);
}

void SpriteRenderer::render(const Command &command)
{
    const Sprite &sprite = sprites[command.sprite_index];

    glBindTexture(GL_TEXTURE_2D, sprite.diffuse_texture_id);
    // std::cout << "Tex id: " << sprite.diffuse_texture_id << std::endl;

    glUniformMatrix4fv(m_loc, 1, GL_FALSE, &(*command.model)[0][0]);
    auto vpos = (*command.model) * glm::vec4(0, 0, 0, 1);
    // std::cout << "Pos : " << vpos.x << ", " << vpos.y << ", " << vpos.z << std::endl;

    // std::cout << "Vertices offset: " << sprite.vertices_offset << std::endl;
    // std::cout << "Indices offset: " << sprite.indices_offset << std::endl;

    glDrawElementsBaseVertex(
        GL_TRIANGLES, 6, GL_UNSIGNED_SHORT,
        (void*)(sizeof(unsigned short) * sprite.indices_offset),
        sprite.vertices_offset
    );
}

