#include "game/systems.h"

void update_entity(component::Transform &transform, component::Physics &physics, double dt) {
    transform.pos.x += physics.twist.x * dt;
    transform.pos.y += physics.twist.y * dt;
    transform.orientation += physics.twist.z * dt;
    if (transform.orientation < -M_PI) transform.orientation += 2*M_PI;
    if (transform.orientation > M_PI) transform.orientation -= 2*M_PI;
}

void system_physics(EntityManager &entity_manager, double dt) {
    component::Transform *transform;
    component::Physics *physics;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::PHYSICS)) continue;
        transform = entity_manager.get_transform_component(i, 0);
        physics = entity_manager.get_physics_component(i, 0);
        update_entity(*transform, *physics, dt);
    }
};

