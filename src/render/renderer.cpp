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

    // Must have loaded all sprites

    glGenVertexArrays(1, &static_VAO);
    glGenBuffers(1, &static_VBO);
    glGenBuffers(1, &static_EBO);

    glBindVertexArray(static_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, static_VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_vertices.size() * sizeof(Vertex),
        &static_vertices[0],
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_EBO);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER,
        static_indices.size() * sizeof(unsigned short),
        &static_indices[0],
        GL_STATIC_DRAW
    );

    // Attribute 0: Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)0
    );

    // Attribute 1: Texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(Vertex),
        (void*)offsetof(Vertex, tex_coords)
    );
}

void SpriteRenderer::use(const Params &params)
{
    glBindVertexArray(static_VAO);
    glUseProgram(program_id);
    glUniformMatrix4fv(v_loc, 1, GL_FALSE, &params.view[0][0]);
}

void SpriteRenderer::render(const Command &command)
{
    const Sprite &sprite = sprites[command.sprite_index];

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(diffuse_texture_loc, 0);
    glBindTexture(GL_TEXTURE_2D, sprite.diffuse_texture_id);

    glUniformMatrix4fv(m_loc, 1, GL_FALSE, &command.model[0][0]);

    glDrawElements(
        GL_TRIANGLES, 6, GL_UNSIGNED_SHORT,
        (void*)(sizeof(unsigned short) * sprite.indices_offset)
    );

    for (int i = 0; i < static_indices.size(); i++) {
        Vertex v = static_vertices[static_indices[i]];
        glm::vec4 vpos((float)v.position[0], (float)v.position[1], 0, 1.0f);
        glm::mat4 view = glm::scale(glm::vec3(0.02f, 0.02f, 1)) * glm::rotate(0.0f, glm::vec3(0, 0, 1));
        vpos = view * command.model * vpos;
        // std::cout << "X: " << vpos[0] << ", Y: " << vpos[1] << ", Z: " << vpos[2] << ", W: " << vpos[3] << std::endl;
    }
    // std::cout << "Error: " << glGetError() << std::endl;
}

// RENDERER

Renderer::Renderer(const std::string &base_dir):
    texture_manager(base_dir),
    shaders(base_dir)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // glEnable(GL_CULL_FACE);
}

void Renderer::initialise()
{
    sprite_renderer.initialise(shaders.sprite_program_id);

    {
        SpriteConfig config;
        config.texture_id = texture_manager.get_texture_id("spritesheet1.png");
        config.offset = glm::vec2(0, 0);

        config.pos = glm::vec2(0, 0);
        config.size = glm::vec2(256, 256);
        sprite_renderer.load_sprite(config);
    }
}

void Renderer::render()
{
    glClearColor(0.6f, 0.6f, 0.6f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render sprites
    
    glm::mat4 view = glm::scale(glm::vec3(0.002f, 0.002f, 1)) * glm::rotate(0.0f, glm::vec3(0, 0, 1));
    SpriteRenderer::Params params = { view };
    sprite_renderer.use(params);

    double x = 0;
    double y = 0;
    double depth = 1;
    double orientation = 0;

    glm::mat4 model =
        glm::translate(glm::vec3(x, y, depth))
        * glm::rotate((float)orientation, glm::vec3(0, 0, 1));

    SpriteRenderer::Command command = { model, 0 };
    sprite_renderer.render(command);
}
