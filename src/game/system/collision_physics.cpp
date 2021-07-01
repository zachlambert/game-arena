#include "game/system.h"

void update_entity(
    component::Transform &transform,
    const component::Physics &physics,
    component::Hitbox &hitbox)
{
    if (hitbox.collisions.empty()) return;

    // Resolve collisions
    hitbox.collisions.clear();
    transform.pos.x -= physics.displacement.x;
    transform.pos.y -= physics.displacement.y;
    transform.orientation -= physics.displacement.z;
}

void system_collision_physics(EntityManager &entity_manager)
{
    component::Transform *transform;
    component::Physics *physics;
    component::Hitbox *hitbox;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::COLLISION_PHYSICS)) continue;
        transform = entity_manager.get_transform_component(i, 0);
        physics = entity_manager.get_physics_component(i, 0);
        hitbox = entity_manager.get_hitbox_component(i, 0);
        update_entity(*transform, *physics, *hitbox);
    }
}
