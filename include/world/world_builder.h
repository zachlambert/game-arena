#ifndef WORLD_WORLD_LOADER_H
#define WORLD_WORLD_LOADER_H

#include "world/entity.h"
#include "world/entity_builder.h"

void build_world(EntityManager &entity_manager)
{
    // Create player
    {
        PlayerConfig config;
        config.sprite_index = 4;
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

    // Create a terrain element
    {
        TerrainConfig config;
        config.mesh_index = 0;
        config.pos = glm::vec2(-200, 0);
        config.orientation = M_PI/2;
        build_terrain(entity_manager, config);
    }
}

#endif
