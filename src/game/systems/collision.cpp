#include "game/systems.h"
#include "game/collision.h"

#include <iostream>

static void update_entity(const component::Transform &transform, component::Hitbox &hitbox, CollisionManager &collision_manager)
{
    // 1. Update hitbox (original hitbox bounds the collision mesh over all orientations)
    double scale = (transform.scale.x > transform.scale.y ? transform.scale.x : transform.scale.y);
    hitbox.box.right = transform.pos.x + scale * hitbox.original_box.right;
    hitbox.box.left = transform.pos.x + scale * hitbox.original_box.left;
    hitbox.box.right = transform.pos.y + scale * hitbox.original_box.top;
    hitbox.box.right = transform.pos.y + scale * hitbox.original_box.bot;

    // 2. Check for intersections
    std::vector<Intersection> intersections;
    collision_manager.check_terrain_entity(transform, hitbox, intersections);
    for (const auto &intersection: intersections) {
        std::cout << "Intersection at (" << intersection.pos.x << ", " << intersection.pos.y << ")" << std::endl;
    }
}

void system_collision(EntityManager &entity_manager, CollisionManager &collision_manager)
{
    component::Transform *transform;
    component::Hitbox *hitbox;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::COLLISION)) continue;
        transform = entity_manager.get_transform_component(i, 0);
        hitbox = entity_manager.get_hitbox_component(i, 0);
        update_entity(*transform, *hitbox, collision_manager);
    }
};

