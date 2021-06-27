#include "render/renderer.h"

#include <iostream>

void Renderer::initialise(const Game &game)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    sprite_renderer.initialise(shaders.sprite_program_id);
    mesh_renderer.initialise(shaders.mesh_program_id);
    terrain_renderer.initialise(shaders.terrain_program_id);
}

void Renderer::render(const Game &game)
{
    glClearColor(0.9f, 0.9f, 0.9f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    game.camera.update_view_matrix();

    // Render terrain
    {
        TerrainRenderer::Params params;
        params.view = &game.camera.view;
        terrain_renderer.enable(params);
        terrain_renderer.render();
    }

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
