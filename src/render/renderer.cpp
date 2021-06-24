#include "render/renderer.h"
#include "world/world.h"

#include <iostream>

void initialise_texture_manager(TextureManager &texture_manager)
{
    texture_manager.load_texture("spritesheet1_diffuse", "spritesheet1.png");
}

void initialise_sprite_renderer(SpriteRenderer &renderer, const TextureManager &texture_manager, const Shaders &shaders)
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

void initialise_mesh_renderer(MeshRenderer &renderer, const Shaders &shaders)
{
    // Remember: Define meshes clockwise !

    std::vector<glm::vec2> vertices;
    glm::vec4 color;

    vertices.push_back(glm::vec2(0, 0));
    vertices.push_back(glm::vec2(0, 100));
    vertices.push_back(glm::vec2(-100, 120));
    vertices.push_back(glm::vec2(-100, 140));
    vertices.push_back(glm::vec2(-50, 160));
    vertices.push_back(glm::vec2(100, 160));
    vertices.push_back(glm::vec2(100, 200));
    vertices.push_back(glm::vec2(80, 200));
    vertices.push_back(glm::vec2(80, 250));
    vertices.push_back(glm::vec2(150, 250));
    vertices.push_back(glm::vec2(150, 50));
    vertices.push_back(glm::vec2(160, 450));
    vertices.push_back(glm::vec2(120, 420));
    vertices.push_back(glm::vec2(110, 380));
    vertices.push_back(glm::vec2(120, 390));
    vertices.push_back(glm::vec2(90, 390));
    vertices.push_back(glm::vec2(90, 480));
    vertices.push_back(glm::vec2(160, 470));
    vertices.push_back(glm::vec2(200, 400));
    // vertices.push_back(glm::vec2(250, 300));
    // vertices.push_back(glm::vec2(200, 200));
    vertices.push_back(glm::vec2(300, 0));
    color = glm::vec4(1, 0, 0, 1);
    renderer.load_mesh(triangulate_mesh(vertices, color));

    renderer.initialise(shaders.mesh_program_id);
}

void Renderer::initialise()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    initialise_texture_manager(texture_manager);
    initialise_sprite_renderer(sprite_renderer, texture_manager, shaders);
    initialise_mesh_renderer(mesh_renderer, shaders);
}

void Renderer::render(const World &world)
{
    glClearColor(0.9f, 0.9f, 0.9f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render sprites
    {
        SpriteRenderer::Params params;
        params.view = &world.get_camera().view;
        sprite_renderer.enable(params);

        SpriteRenderer::Command command;
        for (const auto &visual: world.get_visuals()) {
            if (visual.type == VisualType::SPRITE) {
                command.sprite_index = visual.render_index;
                command.model = &visual.model;
                sprite_renderer.render(command);
            }
        }
    }

    // Render meshes
    {
        MeshRenderer::Params params;
        params.view = &world.get_camera().view;
        mesh_renderer.enable(params);

        MeshRenderer::Command command;
        for (const auto &visual: world.get_visuals()) {
            if (visual.type == VisualType::MESH) {
                command.mesh_index = visual.render_index;
                command.model = &visual.model;
                mesh_renderer.render(command);
            }
        }
    }

    // std::cout << "Error: " << glGetError() << std::endl;
}
