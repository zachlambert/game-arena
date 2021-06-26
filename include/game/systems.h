#ifndef GAME_SYSTEMS_H
#define GAME_SYSTEMS_H

#include <array>
#include <iostream>
#include <random>

#include "window/input.h"
#include "game/components.h"
#include "game/camera.h"
#include "game/entity.h"

void system_enemy_spawner(EntityManager &entity_manager, const CollisionManager &collision_manager, double dt);

void system_player(EntityManager &entity_manager, const Input &input, Camera &camera);
void system_enemy(EntityManager &entity_manager);
void system_gunshot(EntityManager &entity_manager);

void system_physics(EntityManager &entity_manager, double dt);
void system_collision(EntityManager &entity_manager, CollisionManager &collision_manager);

void system_render_base(EntityManager &entity_manager) ;
void system_render_gun_ray(EntityManager &entity_manager, const Camera &camera, double dt);

#endif
