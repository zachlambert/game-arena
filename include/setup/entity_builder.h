#ifndef GAME_ENTITY_BUILDER_H
#define GAME_ENTITY_BUILDER_H

#include "setup/sprite_id.h"
#include "game/entity.h"
#include "game/collision.h"

struct PlayerConfig {
    SpriteId sprite_id;
    glm::vec2 start_pos;
    double start_orientation;
};
void build_player(EntityManager &entity_manager, PlayerConfig config);

struct EnemyConfig {
    SpriteId sprite_id;
    glm::vec2 start_pos;
    double start_orientation;
};
void build_enemy(EntityManager &entity_manager, EnemyConfig config);

struct EnemySpawnerConfig {
    double spawn_timeout;
    glm::vec2 pos;
};
void build_enemy_spawner(EntityManager &entity_manager, EnemySpawnerConfig config);

#endif
