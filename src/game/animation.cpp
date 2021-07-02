#include "game/animation.h"

void AnimationManager::add_animation(AnimationId animation_id, AnimationConfig animation)
{
    if (animations.size() <= (int)animation_id) {
        animations.resize((int)animation_id+1);
    }
    animations[(int)animation_id] = animation;
}

const AnimationConfig &AnimationManager::get_animation(AnimationId animation_id)
{
    return animations[(int)animation_id];
}
