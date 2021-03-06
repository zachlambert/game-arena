#include "setup/world_builder.h"

#include <iostream>

void build_world(EntityManager &entity_manager, Terrain &terrain)
{
    // Create player
    {
        PlayerConfig config;
        config.sprite_id = SpriteId::HUMAN_GUN;
        config.start_pos = glm::vec2(0, 0);
        config.start_orientation = 0;
        build_player(entity_manager, config);
    }

    // Create an enemy spawner
    {
        EnemySpawnerConfig config;
        config.pos.x = 0;
        config.pos.y = -100;
        config.spawn_timeout = 2;
        build_enemy_spawner(entity_manager, config);
    }

    // Create occlusion polygon
    {
        build_occlusion_polygon(entity_manager);
    }

    // Build terrain
    {
        terrain.elements.clear();

        // Remember: Define meshes clockwise !
        TerrainElement element;
        element.visual_depth = 6;

        element.pos.x = 400;
        element.pos.y = 300;
        element.vertices.push_back(glm::vec2(-100, 0));
        element.vertices.push_back(glm::vec2(-80, 200));
        element.vertices.push_back(glm::vec2(20, 240));
        element.vertices.push_back(glm::vec2(0, -150));
        element.color = glm::vec4(0, 0, 0.2, 1);
        terrain.elements.push_back(element);

        element.pos.x = -800;
        element.pos.y = 100;
        element.vertices.clear();
        element.vertices.push_back(glm::vec2(-100, 0));
        element.vertices.push_back(glm::vec2(100, 100));
        element.vertices.push_back(glm::vec2(80, -200));
        element.color = glm::vec4(0, 0, 0.2, 1);
        terrain.elements.push_back(element);

        element.pos.x = -100;
        element.pos.y = 600;
        element.vertices.clear();
        element.vertices.push_back(glm::vec2(-400, -40));
        element.vertices.push_back(glm::vec2(-400, 40));
        element.vertices.push_back(glm::vec2(300, 40));
        element.vertices.push_back(glm::vec2(300, -40));
        element.color = glm::vec4(0, 0, 0.2, 1);
        terrain.elements.push_back(element);

        element.pos.x = -100;
        element.pos.y = -400;
        element.vertices.clear();
        element.vertices.push_back(glm::vec2(-500, 100));
        element.vertices.push_back(glm::vec2(100, 20));
        element.vertices.push_back(glm::vec2(400, 80));
        element.vertices.push_back(glm::vec2(-100, -270));
        element.color = glm::vec4(0, 0, 0.2, 1);
        terrain.elements.push_back(element);

        terrain.centre = {0, 0};
        terrain.size = {5000, 5000};
    }
    return;
}

