#include "game/game.h"

#include "setup/world_builder.h"
#include "game/system.h"

#include <iostream>

void Game::initialise(const Resources &resources)
{
    collision_manager.load_sprite_polygons(resources.spritesheets);

    build_world(entity_manager, terrain);

    collision_manager.initialise_terrain(terrain);
}

void Game::update(double dt)
{
    system_enemy_spawner(entity_manager, collision_manager, dt);

    system_player(entity_manager, input, camera);
    system_enemy(entity_manager);
    system_gun(entity_manager, dt, camera);

    system_physics(entity_manager, dt);
    system_collision(entity_manager, collision_manager);

    system_render_sprite(entity_manager);
    system_render_polygon(entity_manager);

    entity_manager.remove_entities();

    // Update camera
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::PLAYER)) continue;
        component::Transform &transform = *entity_manager.get_transform_component(i);
        camera.pos = transform.pos;
        break;
    }
    camera.adjust_zoom(input.get_scroll_amount());
    camera.update_view_matrix();
}
