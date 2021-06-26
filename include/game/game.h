#ifndef GAME_GAME_H
#define GAME_GAME_H

#include <vector>

#include "game/camera.h"
#include "game/entity.h"
#include "game/terrain.h"
#include "window/window.h"
#include "window/input.h"

class Game {
public:
    Game(const Window &window): input(window), camera(window.width, window.height) {}

    void initialise();
    void update(double dt);

    Input input;
    Camera camera;
    EntityManager entity_manager;
    Terrain terrain;
    CollisionManager collision_manager;
};

#endif
