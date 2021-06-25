#include "game/game.h"

#include "game/world_builder.h"
#include <iostream>
#include <random>

void Game::create_default_world()
{
    build_world(entity_manager);
}

void Game::update(double dt)
{
    system_player.update(entity_manager, input, camera);
    system_enemy.update(entity_manager);
    system_gunshot.update(entity_manager);
    system_physics.update(entity_manager, dt);
    system_render_static.update(entity_manager);
    system_render_gun_ray.update(entity_manager, camera, dt);

    // Remove entities if marked for removal
    // TODO
    
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
