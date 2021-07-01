#include "game/system.h"
#include "game/collision.h"

#include <iostream>

static void update_entity(
    const component::Transform &transform,
    const component::Sprite &sprite,
    component::Hitbox &hitbox,
    CollisionManager &collision_manager)
{
    if (sprite.sprite_id == SpriteId::NONE) return;
    if (hitbox.sprite_id != sprite.sprite_id) {
        collision_manager.get_sprite_hitbox(sprite.sprite_id, hitbox);
    }

    // 1. Update hitbox (original hitbox bounds the collision mesh over all orientations)
    double scale = (transform.scale.x > transform.scale.y ? transform.scale.x : transform.scale.y);
    hitbox.box.right = transform.pos.x + scale * hitbox.original_box.right;
    hitbox.box.left = transform.pos.x + scale * hitbox.original_box.left;
    hitbox.box.top = transform.pos.y + scale * hitbox.original_box.top;
    hitbox.box.bot = transform.pos.y + scale * hitbox.original_box.bot;

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
    component::Sprite *sprite;
    component::Hitbox *hitbox;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::COLLISION)) continue;
        transform = entity_manager.get_transform_component(i, 0);
        sprite = entity_manager.get_sprite_component(i, 0);
        hitbox = entity_manager.get_hitbox_component(i, 0);
        update_entity(*transform, *sprite, *hitbox, collision_manager);
    }
};

