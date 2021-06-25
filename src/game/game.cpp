#include "game/game.h"

#include "game/world_builder.h"
#include "game/systems.h"

void Game::create_default_world()
{
    build_world(entity_manager);
}

void Game::update(double dt)
{
    system_player(entity_manager, input, camera);
    system_enemy(entity_manager);
    system_gunshot(entity_manager);
    system_physics(entity_manager, dt);
    system_render_base(entity_manager);
    system_render_gun_ray(entity_manager, camera, dt);

    // Remove entities if marked for removal
    int i = 0;
    while (i < entity_manager.entities.tail) {
        if (entity_manager.entities[i].to_remove) {
            entity_manager.entity_remove(i);
        } else {
            i++;
        }
    }
    
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
