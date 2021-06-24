#include "render/renderer.h"

#include <iostream>
#include <iterator>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

Vertex::Vertex(glm::vec3 position, glm::vec2 tex_coords)
{
    // Position straightforward
    this->position = position;
    // Tex coords: Need to convert to normalised ushort
    // If outside the [0, 1] range, clamp coordinates
    this->tex_coords[0] = static_cast<unsigned short>(
        glm::clamp(tex_coords.x, 0.0f, 1.0f) * (float)0xFFFF);
    this->tex_coords[1] = static_cast<unsigned short>(
        glm::clamp(tex_coords.y, 0.0f, 1.0f) * (float)0xFFFF);
}

void SpriteRenderer::load_sprite(const SpriteConfig &config)
{
    double left = -(config.size[0] + config.offset[0])/2;
    double bot = -(config.size[1] + config.offset[1])/2;
    double right = (config.size[0] - config.offset[0])/2;
    double top = (config.size[1] - config.offset[1])/2;

    // TODO: Change denominator to spritesheet size when has more than one sprite
    double uv_left = config.pos[0]/config.size[0];
    double uv_bot = config.pos[1]/config.size[1];
    double uv_right = (config.pos[0]+config.size[0])/config.size[0];
    double uv_top = (config.pos[1]+config.size[1])/config.size[1];

    unsigned int vertices_offset = static_vertices.size();

    static_vertices.push_back(Vertex(
        glm::vec3(left, bot, 0),
        glm::vec2(uv_left, uv_bot)
    ));
    static_vertices.push_back(Vertex(
        glm::vec3(right, bot, 0),
        glm::vec2(uv_right, uv_bot)
    ));
    static_vertices.push_back(Vertex(
        glm::vec3(right, top, 0),
        glm::vec2(uv_right, uv_top)
    ));
    static_vertices.push_back(Vertex(
        glm::vec3(left, top, 0),
        glm::vec2(uv_left, uv_top)
    ));

    unsigned int indices_offset = static_indices.size();

    static_indices.push_back(0);
    static_indices.push_back(1);
    static_indices.push_back(2);
    static_indices.push_back(0);
    static_indices.push_back(2);
    static_indices.push_back(3);

    sprites.push_back({config.texture_id, vertices_offset, indices_offset});
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

    glUniformMatrix4fv(m_loc, 1, GL_FALSE, &(*command.model)[0][0]);

    glDrawElementsBaseVertex(
        GL_TRIANGLES, 6, GL_UNSIGNED_SHORT,
        (void*)(sizeof(unsigned short) * sprite.indices_offset),
        sprite.vertices_offset
    );

    // std::cout << "Error: " << glGetError() << std::endl;
}

// RENDERER

Renderer::Renderer(const std::string &base_dir):
    texture_manager(base_dir),
    shaders(base_dir)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::initialise()
{
    {
        SpriteConfig config;
        config.texture_id = texture_manager.get_texture_id("spritesheet1.png");
        config.offset = glm::vec2(0, 0);

        config.pos = glm::vec2(0, 0);
        config.size = glm::vec2(256, 256);
        sprite_renderer.load_sprite(config);
    }

    sprite_renderer.initialise(shaders.sprite_program_id);
}

void Renderer::render(const World &world)
{
    glClearColor(0.6f, 0.6f, 0.6f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    world.camera.update_view_matrix();
    for (const auto &entity: world.entities) {
        entity.update_model_matrix();
    }

    // Draw sprites
    SpriteRenderer::Params sprite_params;
    sprite_params.view = &world.camera.view;
    sprite_renderer.enable(sprite_params);

    SpriteRenderer::Command sprite_command;
    for (const auto &entity: world.entities) {
        if (entity.sprite_index >= 0) {
            sprite_command.sprite_index = entity.sprite_index;
            sprite_command.model = &entity.model;
            sprite_renderer.render(sprite_command);
        }
    }
}
