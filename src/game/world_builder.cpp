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

    // Create an enemy
    {
        EnemyConfig config;
        config.sprite_index = 0;
        config.start_pos = glm::vec2(100, 100);
        config.start_orientation = 0.5;
        build_enemy(entity_manager, config);
    }

    // Create a gun ray
    {
        // x_scale = length
        // y_scale = tan(theta/2)*length
        TerrainConfig config;
        config.mesh_index = 0;
        config.pos = glm::vec2(-200, 0);
        config.orientation = M_PI/2;
        build_terrain(entity_manager, config);
    }

    // Create an enemy spawner
    {
        EnemySpawnerConfig config;
        config.pos.x = 0;
        config.pos.y = -100;
        config.spawn_timeout = 0.01;
        build_enemy_spawner(entity_manager, config);
    }
    return;
}

