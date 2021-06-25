#include "game/game.h"

#include "game/world_builder.h"
#include <iostream>
#include <random>

void Game::create_default_world()
{
    build_world(entity_manager);
}

void Game::update(double dt)
{
    system_player.update(entity_manager, input, camera);
    system_enemy.update(entity_manager);
    system_physics.update(entity_manager, dt);
    system_render_static.update(entity_manager);
}
