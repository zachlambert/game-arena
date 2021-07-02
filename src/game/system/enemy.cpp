#include "game/system.h"
#include <random>

static void update_physics(component::Physics &physics)
{
    static std::default_random_engine generator;
    static std::normal_distribution<double> distribution(0, 1);
    physics.twist.x = physics.twist.x*0.99 + distribution(generator)*25;
    physics.twist.y = physics.twist.y*0.99 + distribution(generator)*25;
    physics.twist.z = physics.twist.z*0.99 + distribution(generator)*0.3;
}

static void update_animation(const component::Physics &physics, component::Animation &animation)
{
    if (!animation.started) {
        animation.started = true;
        animation.id = AnimationId::HUMAN_WALK;
        animation.index = -1;
        animation.frame_time = 0.2;
        animation.loop = true;
    }
    // animation.frame_time = 0.25 * ... Use walk speed
}

void system_enemy(EntityManager &entity_manager)
{
    component::Physics *physics;
    component::Animation *animation;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::ENEMY)) continue;
        physics = entity_manager.get_physics_component(i, 0);
        animation = entity_manager.get_animation_component(i, 0);
        update_physics(*physics);
        update_animation(*physics, *animation);
    }
};

