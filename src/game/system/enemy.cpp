#include "game/system.h"
#include <random>

void update_entity(component::Physics &physics)
{
    static std::default_random_engine generator;
    static std::normal_distribution<double> distribution(0, 1);
    physics.twist.x = physics.twist.x*0.99 + distribution(generator)*25;
    physics.twist.y = physics.twist.y*0.99 + distribution(generator)*25;
    physics.twist.z = physics.twist.z*0.99 + distribution(generator)*0.5;
}

void system_enemy(EntityManager &entity_manager)
{
    component::Physics *physics;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::ENEMY)) continue;
        physics = entity_manager.get_physics_component(i, 0);
        update_entity(*physics);
    }
};

