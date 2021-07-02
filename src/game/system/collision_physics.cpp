#include "game/system.h"

void resolve_collisions_soft(
    component::Transform &transform,
    component::Physics &physics,
    component::Hitbox &hitbox)
{
    constexpr float kp = 10000;
    constexpr float kd = 200;
    glm::vec2 contact_vel;
    glm::vec2 v0(physics.twist.x, physics.twist.y);
    glm::vec2 disp, perp;
    for (const auto &collision: hitbox.collisions) {
        disp = collision.pos - transform.pos;
        perp = {-disp.y, disp.x};
        contact_vel = glm::vec2(physics.twist.x, physics.twist.y) + perp*physics.twist.z;
        float d_depth = -glm::dot(contact_vel, collision.normal);
        physics.contact_force.x += kp * collision.normal.x * collision.depth;
        physics.contact_force.y += kp * collision.normal.y * collision.depth;
        physics.contact_force.x += kd * collision.normal.x * d_depth;
        physics.contact_force.y += kd * collision.normal.y * d_depth;
        physics.contact_force.z += disp.x*physics.contact_force.y - disp.y*physics.contact_force.x;
    }
    hitbox.collisions.clear();
}

void update_entity(
    component::Transform &transform,
    component::Physics &physics,
    component::Hitbox &hitbox)
{
    if (hitbox.collisions.empty()) return;
    resolve_collisions_soft(transform, physics, hitbox);
    return;

    if (hitbox.collisions.size() == 1) {
        transform.pos += hitbox.collisions[0].normal * (float)hitbox.collisions[0].depth;
        hitbox.collisions.clear();
        return;
    }

    if (hitbox.collisions.size() > 2) {
        resolve_collisions_soft(transform, physics, hitbox);
        return;
    }

    const glm::vec2 &n1 = hitbox.collisions[0].normal;
    double d1 = hitbox.collisions[0].depth;
    const glm::vec2 &n2 = hitbox.collisions[1].normal;
    double d2 = hitbox.collisions[1].depth;
    double dot = glm::dot(n1, n2);

    // Contacts in opposite directions, away from each other.
    // Occurs when an entity penetrates through a small bit of terrain.
    // In this case, ignore the larger displacement.
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

    if (dot > 1-1e-6) {
        if (d1 > d2) {
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
    if (hypot(disp.x, disp.y) > 5 || std::isinf(a) || std::isinf(b) ) {
        // If the required displacement to resolve the collision is large, just
        // revert the displacement instead. This avoids large jumps when rotating
        // beteen two walls either side.
        transform.pos.x -= physics.displacement.x;
        transform.pos.y -= physics.displacement.y;
        transform.orientation -= physics.displacement.z;
        std::cout << "asdf" << std::endl;
    } else {
        transform.pos += a*n1 + b*n2;
        std::cout << hypot(disp.x, disp.y) << std::endl;
        std::cout << "d1 = " << d1 << std::endl;
        std::cout << "d2 = " << d1 << std::endl;
        std::cout << "dot = " << dot << std::endl;
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
