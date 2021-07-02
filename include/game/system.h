#ifndef GAME_SYSTEMS_H
#define GAME_SYSTEMS_H

#include <array>
#include <iostream>
#include <random>

#include "window/input.h"
#include "game/components.h"
#include "game/camera.h"
#include "game/entity.h"
#include "game/animation.h"

void system_enemy_spawner(EntityManager &entity_manager, const CollisionManager &collision_manager, double dt);

void system_player(EntityManager &entity_manager, const Input &input, Camera &camera, double dt);
void system_enemy(EntityManager &entity_manager);
void system_gun(EntityManager &entity_manager, double dt, const Camera &camera, const CollisionManager &collision_manager);

void system_physics(EntityManager &entity_manager, double dt);
void system_collision(EntityManager &entity_manager, CollisionManager &collision_manager);
void system_collision_physics(EntityManager &entity_manager);

void system_occlusion_polygon(EntityManager &entity_manager, const CollisionManager &collision_manager, const Camera &camera);

void system_animation(EntityManager &entity_manager, double dt, const AnimationManager &animation_manager);
void system_render_sprite(EntityManager &entity_manager);
void system_render_polygon(EntityManager &entity_manager);

#endif
