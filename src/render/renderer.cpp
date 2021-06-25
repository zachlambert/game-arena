#include "render/renderer.h"

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

    // Normalised gun ray
    // x_scale = length
    // y_scale = tan(theta/2)*length
    vertices.push_back(glm::vec2(0, 0));
    vertices.push_back(glm::vec2(100, 100));
    vertices.push_back(glm::vec2(100, -100));
    color = glm::vec4(0.5, 0.5, 0.5, 1);
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

void Renderer::render(const Game &game)
{
    glClearColor(0.9f, 0.9f, 0.9f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    game.camera.update_view_matrix();

    // Render sprites
    {
        SpriteRenderer::Params params;
        params.view = &game.camera.view;
        sprite_renderer.enable(params);

        SpriteRenderer::Command command;
        for (int i = 0; i < game.entity_manager.visual_static.tail; i++) {
            const component::VisualStatic &visual_static = game.entity_manager.visual_static[i];
            if (visual_static.type == component::VisualStatic::Type::SPRITE) {
                command.sprite_index = visual_static.render_index;
                command.model = &visual_static.model;
                sprite_renderer.render(command);
            }
        }
    }

    // Render meshes
    {
        MeshRenderer::Params params;
        params.view = &game.camera.view;
        mesh_renderer.enable(params);

        MeshRenderer::Command command;
        for (int i = 0; i < game.entity_manager.visual_static.tail; i++) {
            const component::VisualStatic &visual_static = game.entity_manager.visual_static[i];
            if (visual_static.type == component::VisualStatic::Type::MESH) {
                command.mesh_index = visual_static.render_index;
                command.model = &visual_static.model;
                mesh_renderer.render(command);
            }
        }
    }

    // std::cout << "Error: " << glGetError() << std::endl;
}
