#include "game/system.h"

static void update_entity(component::Transform &transform, component::Physics &physics, double dt) {
    physics.displacement.x = physics.twist.x * dt;
    physics.displacement.y = physics.twist.y * dt;
    physics.displacement.z = physics.twist.z * dt;
    transform.pos.x += physics.displacement.x;
    transform.pos.y += physics.displacement.y;
    transform.orientation += physics.displacement.z;
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

