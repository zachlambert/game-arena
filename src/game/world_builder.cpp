#include "game/world_builder.h"

#include <iostream>

void build_world(EntityManager &entity_manager, Terrain &terrain, const CollisionManager &collision_manager)
{
    // Create player
    {
        PlayerConfig config;
        config.sprite_index = 3;
        config.gun_ray_mesh_index_aiming = 1;
        config.gun_ray_mesh_index_fired = 2;
        config.start_pos = glm::vec2(0, 0);
        config.start_orientation = 0;
        build_player(entity_manager, collision_manager, config);
    }

    // Create an enemy spawner
    {
        EnemySpawnerConfig config;
        config.pos.x = 0;
        config.pos.y = -100;
        config.spawn_timeout = 1.5;
        build_enemy_spawner(entity_manager, config);
    }

    // Build terrain
    {
        terrain.elements.clear();

        // Remember: Define meshes clockwise !
        TerrainElement element;
        element.pos.x = 0;
        element.pos.y = 0;
        element.visual_depth = 10;
        element.color = glm::vec4(0, 0, 0.2, 1);

        element.vertices.push_back(glm::vec2(-100, 0));
        element.vertices.push_back(glm::vec2(-80, 200));
        element.vertices.push_back(glm::vec2(20, 240));
        element.vertices.push_back(glm::vec2(0, -150));
        element.color = glm::vec4(0, 0, 0.2, 1);

        terrain.elements.push_back(element);
    }
    return;
}

