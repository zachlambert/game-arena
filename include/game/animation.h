#ifndef GAME_ANIMATION_H
#define GAME_ANIMATION_H

#include <vector>

#include "setup/sprite_id.h"
#include "setup/animation_id.h"

struct AnimationConfig {
    std::vector<SpriteId> sprites;
};

class AnimationManager {
public:
    void add_animation(AnimationId animation_id, AnimationConfig animation);
    const AnimationConfig &get_animation(AnimationId animation_id)const;
    void increment_frame(AnimationId animation_id, int &index, SpriteId &sprite_id, bool &looped)const;
private:
    std::vector<AnimationConfig> animations;
};

#endif
