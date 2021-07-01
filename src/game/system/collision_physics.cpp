#include "game/system.h"

void update_entity(
    component::Transform &transform,
    const component::Physics &physics,
    component::Hitbox &hitbox)
{
    if (hitbox.collisions.empty()) return;

    // Resolve collisions
    if (hitbox.collisions.size() > 2) {
        // Shouldn't be able to happen
        transform.pos.x -= physics.displacement.x;
        transform.pos.y -= physics.displacement.y;
        transform.orientation -= physics.displacement.z;
        hitbox.collisions.clear();
        return;
    }

    if (hitbox.collisions.size() == 1) {
        transform.pos += hitbox.collisions[0].normal * (float)hitbox.collisions[0].depth;
        hitbox.collisions.clear();
        return;
    }

    const glm::vec2 &n1 = hitbox.collisions[0].normal;
    double d1 = hitbox.collisions[0].depth;
    const glm::vec2 &n2 = hitbox.collisions[1].normal;
    double d2 = hitbox.collisions[1].depth;
    double dot = glm::dot(n1, n2);

    if (dot < 0 && glm::dot(hitbox.collisions[1].pos - hitbox.collisions[0].pos, n1) < 0) {
        glm::vec2 disp(physics.displacement.x, physics.displacement.y);
        if (glm::dot(disp, n1) < 0) {
            transform.pos += n1 * (float)d1;
        } else {
            transform.pos += n2 * (float)d2;
        }
        hitbox.collisions.clear();
        return;
    }

    float a = (d1 - d2*dot) / (1 - dot*dot);
    float b = d2 - a*dot;

    glm::vec2 disp = a*n1 + b*n2;
    if (hypot(disp.x, disp.y) > 50) {
        // If the required displacement to resolve the collision is large, just
        // revert the displacement instead. This avoids large jumps when rotating
        // beteen two walls either side.
        transform.pos.x -= physics.displacement.x;
        transform.pos.y -= physics.displacement.y;
        transform.orientation -= physics.displacement.z;
    } else {
        transform.pos += a*n1 + b*n2;
    }

    hitbox.collisions.clear();
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
