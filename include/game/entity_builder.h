#ifndef GAME_ENTITY_BUILDER_H
#define GAME_ENTITY_BUILDER_H

#include "game/entity.h"

struct PlayerConfig {
    unsigned int sprite_index;
    unsigned int gun_ray_mesh_index_aiming;
    unsigned int gun_ray_mesh_index_fired;
    glm::vec2 start_pos;
    double start_orientation;
};
void build_player(EntityManager &entity_manager, PlayerConfig config);

struct EnemyConfig {
    unsigned int sprite_index;
    glm::vec2 start_pos;
    double start_orientation;
};
void build_enemy(EntityManager &entity_manager, EnemyConfig config);

struct TerrainConfig {
    unsigned int mesh_index;
    glm::vec2 pos;
    double orientation;
};
void build_terrain(EntityManager &entity_manager, TerrainConfig config);

struct EnemySpawnerConfig {
    double spawn_timeout;
    glm::vec2 pos;
};
void build_enemy_spawner(EntityManager &entity_manager, EnemySpawnerConfig config);

#endif
