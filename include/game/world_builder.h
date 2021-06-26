#ifndef GAME_WORLD_LOADER_H
#define GAME_WORLD_LOADER_H

#include "game/entity.h"
#include "game/entity_builder.h"
#include "game/terrain.h"

void build_world(EntityManager &entity_manager, Terrain &terrain, const CollisionManager &collision_manager);

#endif
