#include "game/game.h"

#include "game/world_builder.h"
#include "game/systems.h"

void Game::initialise()
{
    build_world(entity_manager, terrain);
}

void Game::update(double dt)
{
    system_enemy_spawner(entity_manager, dt);

    system_player(entity_manager, input, camera);
    system_enemy(entity_manager);
    system_gunshot(entity_manager);
    system_physics(entity_manager, dt);
    system_render_base(entity_manager);
    system_render_gun_ray(entity_manager, camera, dt);

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
