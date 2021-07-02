#include "game/system.h"
#include "game/animation.h"

static void update_entity(component::Animation &animation, component::Sprite &sprite, double dt, const AnimationManager &animation_manager)
{
    if (!animation.started) return;
    if (animation.index == -1) {
        bool looped; // Unused
        animation_manager.increment_frame(animation.id, animation.index, sprite.sprite_id, looped);
        return;
    }

    animation.frame_timer += dt;
    if (animation.frame_timer < animation.frame_time) return;

    animation.frame_timer = 0;

    bool looped;
    SpriteId next_sprite;
    animation_manager.increment_frame(animation.id, animation.index, next_sprite, looped);

    if (looped && !animation.loop) {
        animation.started = false;
        return;
    }
    sprite.sprite_id = next_sprite;
}

void system_animation(EntityManager &entity_manager, double dt, const AnimationManager &animation_manager)
{
    component::Animation *animation;
    component::Sprite *sprite;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::ANIMATION)) continue;
        animation = entity_manager.get_animation_component(i, 0);
        sprite = entity_manager.get_sprite_component(i, 0);
        update_entity(*animation, *sprite, dt, animation_manager);
    }
};

