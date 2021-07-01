#include "game/system.h"
#include "setup/entity_builder.h"

struct SpawnCommand {
    glm::vec2 pos;
    unsigned int enemy_type; // Unused at the moment
};

void check_for_spawn(component::EnemySpawner &enemy_spawner, double dt, std::vector<SpawnCommand> &spawn_commands)
{
    enemy_spawner.spawn_timer += dt;
    if (enemy_spawner.spawn_timer < enemy_spawner.spawn_timeout) return;

    enemy_spawner.spawn_timer = 0;
    SpawnCommand command;
    command.pos = enemy_spawner.pos;
    spawn_commands.push_back(command);
}

void system_enemy_spawner(EntityManager &entity_manager, const CollisionManager &collision_manager, double dt) {
    std::vector<SpawnCommand> spawn_commands;

    component::EnemySpawner *enemy_spawner;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::ENEMY_SPAWNER)) continue;
        enemy_spawner = entity_manager.get_enemy_spawner_component(i, 0);
        check_for_spawn(*enemy_spawner, dt, spawn_commands);
    }

    for (auto &spawn_command: spawn_commands) {
        EnemyConfig config;
        config.start_pos = spawn_command.pos;
        config.start_orientation = 0;
        config.sprite_id = SpriteId::HUMAN_STILL;
        build_enemy(entity_manager, config);
    }
};
