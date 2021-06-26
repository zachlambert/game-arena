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

    // Terrain element
    {
    std::vector<glm::vec2> vertices;
    glm::vec4 color;
    vertices.push_back(glm::vec2(-100, 0));
    vertices.push_back(glm::vec2(-80, 200));
    vertices.push_back(glm::vec2(20, 240));
    vertices.push_back(glm::vec2(0, -150));
    color = glm::vec4(0, 0, 0.1, 1);
    renderer.load_mesh(triangulate_mesh(vertices, color));
    }

    // Normalised gun ray (aiming)
    // x_scale = length
    // y_scale = tan(theta/2)*length
    {
    MeshConfig config;
    config.colors.push_back(glm::vec4(0.7, 0.7, 0.7, 1));
    config.colors.push_back(glm::vec4(0.7, 0.7, 0.7, -1)); // Negative opacity means it faded to 0 more quickly
    config.colors.push_back(glm::vec4(0.7, 0.7, 0.7, -1));
    config.vertices.push_back(glm::vec2(0, 0));
    config.vertices.push_back(glm::vec2(1, 1));
    config.vertices.push_back(glm::vec2(1, -1));
    config.indices.push_back(0);
    config.indices.push_back(1);
    config.indices.push_back(2);
    renderer.load_mesh(config);
    }

    // Normalised gun ray (fired)
    {
    MeshConfig config;
    config.colors.push_back(glm::vec4(0, 0, 0, 1)); // Darker
    config.colors.push_back(glm::vec4(0, 0, 0, -1));
    config.colors.push_back(glm::vec4(0, 0, 0, -1));
    config.vertices.push_back(glm::vec2(0, 0)); // Thinner
    config.vertices.push_back(glm::vec2(1, 0.8)); 
    config.vertices.push_back(glm::vec2(1, -0.8));
    config.indices.push_back(0);
    config.indices.push_back(1);
    config.indices.push_back(2);
    renderer.load_mesh(config);
    }

    renderer.initialise(shaders.mesh_program_id);
}

void initialise_terrain_renderer(TerrainRenderer &renderer, const Shaders &shaders, const Terrain &terrain)
{
    renderer.load_terrain(terrain);
    renderer.initialise(shaders.terrain_program_id);
}

void Renderer::initialise(const Game &game)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    initialise_texture_manager(texture_manager);
    initialise_sprite_renderer(sprite_renderer, texture_manager, shaders);
    initialise_mesh_renderer(mesh_renderer, shaders);
    initialise_terrain_renderer(terrain_renderer, shaders, game.terrain);
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
