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

class SystemRenderBase {
public:
    void update(EntityManager &entity_manager) ;
private:
    void update_entity(component::Transform &transform, component::VisualStatic &visual_static);
};

class SystemPlayer {
public:
    void update(EntityManager &entity_manager, const Input &input, Camera &camera);
private:
    void update_entity(component::Transform &transform, component::Physics &physics, component::Gun &gun, const Input &input, Camera &camera);
};

class SystemEnemy {
public:
    void update(EntityManager &entity_manager);
private:
    void update_entity(component::Physics &physics);
};

class SystemRenderGunRay {
public:
    void update(EntityManager &entity_manager, const Camera &camera, double dt);
private:
    void update_entity(component::Transform &transform, component::Gun &gun, component::VisualStatic &visual_static, const Camera &camera, double dt);
};

class SystemGunshot {
public:
    void update(EntityManager &entity_manager);
private:
    struct Gunshot {
        glm::vec2 origin;
        glm::vec2 fire_point;
        double focus;
        int entity_id;
    };
    std::vector<Gunshot> gunshots;
    void check_for_gunshot(const component::Transform &transform, const component::Gun &gun, int entity_id);
    bool update_entity(component::Transform &transform, int entity_id);
};

#endif
