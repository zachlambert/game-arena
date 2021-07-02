#include "game/animation.h"

void AnimationManager::add_animation(AnimationId animation_id, AnimationConfig animation)
{
    if (animations.size() <= (int)animation_id) {
        animations.resize((int)animation_id+1);
    }
    animations[(int)animation_id] = animation;
}

void AnimationManager::increment_frame(
    AnimationId animation_id,
    int &index,
    SpriteId &sprite_id,
    bool &looped)const
{
    looped = false;
    index++;
    const AnimationConfig &animation = animations[(int)animation_id];
    if (index == animation.sprites.size()) {
        index = 0;
        looped = true;
    }
    sprite_id = animation.sprites[index];
}
