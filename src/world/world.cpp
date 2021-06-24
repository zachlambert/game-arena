#include "world/world.h"

void create_default_world(World &world)
{
    Entity player;
    player.sprite_index = 0;
    player.depth = -1;
    player.orientation = 0.5;
    world.add_player(player);

    Entity enemy1;
    enemy1.sprite_index = 3;
    enemy1.pos.x = 200;
    enemy1.pos.y = 100;
    enemy1.orientation = -0.78;
    enemy1.depth = -2;
    world.add_entity(enemy1);
}

void World::update(double dt)
{
}
