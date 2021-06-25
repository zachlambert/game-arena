#ifndef GAME_SYSTEMS_H
#define GAME_SYSTEMS_H

#include <array>
#include <iostream>
#include <random>

#include "window/input.h"
#include "game/components.h"
#include "game/camera.h"
#include "game/entity.h"

// Represent systems by classes instead of just functions, in case a system
// needs to store some state.

class SystemPhysics {
public:
    void update(EntityManager &entity_manager, double dt);
private:
    void update_entity(component::Transform &transform, component::Physics &physics, double dt);
};

class SystemRenderStatic {
public:
    void update(EntityManager &entity_manager) ;
private:
    void update_entity(component::Transform &transform, component::VisualStatic &visual_static);
};

class SystemPlayer {
public:
    void update(EntityManager &entity_manager, const Input &input, Camera &camera);
private:
    void update_entity(component::Transform &transform, component::Physics &physics, const Input &input, Camera &camera);
};

class SystemEnemy {
public:
    void update(EntityManager &entity_manager);
private:
    void update_entity(component::Physics &physics);
};

#endif
