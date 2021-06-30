#include "render/renderer.h"

#include <iostream>

void Renderer::initialise(const Resources &resources, const Game &game)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    sprite_renderer.load_spritesheets(base_dir, resources.spritesheets);
    terrain_renderer.load_terrain(game.terrain);
    collision_renderer.load_spritesheets(base_dir, resources.spritesheets);
    collision_renderer.load_terrain_collision_data(game.collision_manager);

    sprite_renderer.initialise(load_shader(
        base_dir + "shaders/sprite.vs",
        base_dir + "shaders/sprite.fs"
    ));
    polygon_renderer.initialise(load_shader(
        base_dir + "shaders/polygon.vs",
        base_dir + "shaders/polygon.fs"
    ));
    terrain_renderer.initialise(load_shader(
        base_dir + "shaders/terrain.vs",
        base_dir + "shaders/terrain.fs"
    ));
    collision_renderer.initialise(
        load_shader(base_dir + "shaders/terrain.vs",
                    base_dir + "shaders/terrain.fs"),
        load_shader(base_dir + "shaders/polygon.vs",
                    base_dir + "shaders/polygon.fs")
    );
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
        for (int i = 0; i < game.entity_manager.sprite.tail; i++) {
            const component::Sprite &sprite = game.entity_manager.sprite[i];
            command.sprite_id = sprite.sprite_id;
            command.model = &sprite.model;
            sprite_renderer.render(command);
        }
    }

    // Render meshes
    {
        polygon_renderer.enable(game.camera.view);
        for (int i = 0; i < game.entity_manager.polygon.tail; i++) {
            const component::Polygon &polygon = game.entity_manager.polygon[i];
            polygon_renderer.store_polygon(polygon);
        }
        polygon_renderer.reinitialise();
        for (int i = 0; i < game.entity_manager.polygon.tail; i++) {
            const component::Polygon &polygon = game.entity_manager.polygon[i];
            polygon_renderer.render(polygon);
        }
    }

    // Debugging

    // Render collision information
    {
        collision_renderer.enable_terrain(game.camera.view);
        collision_renderer.render_terrain();
        collision_renderer.enable_sprite_polygons(game.camera.view);
        for (int i = 0; i < game.entity_manager.sprite.tail; i++) {
            const component::Sprite &sprite = game.entity_manager.sprite[i];
            collision_renderer.render_sprite_polygon(sprite);
        }
    }

    // std::cout << "Error: " << glGetError() << std::endl;
}
