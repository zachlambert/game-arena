#include "game/world_builder.h"

#include <iostream>

void build_world(EntityManager &entity_manager)
{
    // Create player
    {
        PlayerConfig config;
        config.sprite_index = 3;
        config.gun_ray_mesh_index_aiming = 1;
        config.gun_ray_mesh_index_fired = 2;
        config.start_pos = glm::vec2(0, 0);
        config.start_orientation = 0;
        build_player(entity_manager, config);
    }

    // Create an enemy spawner
    {
        EnemySpawnerConfig config;
        config.pos.x = 0;
        config.pos.y = -100;
        config.spawn_timeout = 1.5;
        build_enemy_spawner(entity_manager, config);
    }
    return;
}

