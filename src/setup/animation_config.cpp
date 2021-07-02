#include "setup/animation_config.h"

void load_animations(AnimationManager &animation_manager)
{
    AnimationConfig animation;

    animation.sprites = {SpriteId::HUMAN_WALK_0, SpriteId::HUMAN_WALK_1};
    animation_manager.add_animation(AnimationId::HUMAN_WALK, animation);
}
