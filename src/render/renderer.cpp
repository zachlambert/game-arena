#include "render/renderer.h"
#include "world/world.h"

#include <iostream>

void initialise_texture_manager(TextureManager &texture_manager)
{
    texture_manager.load_texture("spritesheet1_diffuse", "spritesheet1.png");
}

void initialise_sprite_renderer(const TextureManager &texture_manager, const Shaders &shaders, SpriteRenderer &renderer)
{
    SpriteConfig config;
    config.diffuse_texture = texture_manager.get_texture("spritesheet1_diffuse");
    config.offset = glm::vec2(0, 0);

    config.pos = glm::vec2(0, 0);
    config.size = glm::vec2(256, 256);
    renderer.load_sprite(config);

    config.pos = glm::vec2(256, 0);
    renderer.load_sprite(config);

    config.pos = glm::vec2(384, 0);
    renderer.load_sprite(config);

    config.pos = glm::vec2(0, 256);
    config.size = glm::vec2(512, 256);
    config.offset = glm::vec2(-128, 0);
    renderer.load_sprite(config);

    config.pos = glm::vec2(512, 256);
    config.size = glm::vec2(256, 256);
    config.offset = glm::vec2(0, 0);
    renderer.load_sprite(config);

    renderer.initialise(shaders.sprite_program_id);
}

void Renderer::initialise()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initialise_texture_manager(texture_manager);
    initialise_sprite_renderer(texture_manager, shaders, sprite_renderer);
}

void Renderer::render(const World &world)
{
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update view and model matrices
    world.get_camera().update_view_matrix();
    for (const auto &entity: world.get_entities()) {
        entity.update_model_matrix();
    }

    // Draw sprites
    SpriteRenderer::Params sprite_params;
    sprite_params.view = &world.get_camera().view;
    sprite_renderer.enable(sprite_params);

    SpriteRenderer::Command sprite_command;
    for (const auto &entity: world.get_entities()) {
        if (entity.sprite_index >= 0) {
            sprite_command.sprite_index = entity.sprite_index;
            sprite_command.model = &entity.model;
            sprite_renderer.render(sprite_command);
        }
    }

    // std::cout << "Error: " << glGetError() << std::endl;
}
