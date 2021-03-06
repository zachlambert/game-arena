#ifndef GAME_GAME_H
#define GAME_GAME_H

#include <vector>

#include "setup/resources.h"
#include "setup/sprite_config.h"
#include "game/camera.h"
#include "game/entity.h"
#include "game/terrain.h"
#include "game/animation.h"
#include "window/window.h"
#include "window/input.h"

class Game {
public:
    Game(const Window &window): input(window), camera(window.width, window.height) {}

    void initialise(const Resources &resources);
    void update(double dt);
    void cleanup();

    bool paused;
    Input input;
    Camera camera;
    EntityManager entity_manager;
    Terrain terrain;
    CollisionManager collision_manager;
    AnimationManager animation_manager;
};

#endif
