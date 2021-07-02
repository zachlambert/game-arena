#ifndef GAME_ANIMATION_H
#define GAME_ANIMATION_H

#include <vector>

#include "setup/sprite_id.h"
#include "setup/animation_id.h"

struct AnimationConfig {
    std::vector<SpriteId> sprites;
    bool loop;
    double frame_time;
};

class AnimationManager {
public:
    void add_animation(AnimationId animation_id, AnimationConfig animation);
    const AnimationConfig &get_animation(AnimationId animation_id);
private:
    std::vector<AnimationConfig> animations;
};

#endif
