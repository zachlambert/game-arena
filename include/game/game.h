#ifndef GAME_GAME_H
#define GAME_GAME_H

#include <vector>

#include "game/camera.h"
#include "game/entity.h"
#include "window/window.h"
#include "window/input.h"
#include "game/systems.h"

class Game {
public:
    Game(const Window &window): input(window), camera(window.width, window.height) {}

    void create_default_world();
    void update(double dt);
    // const Camera &get_camera()const{ return camera; }
    // const Buffer<component::VisualStatic, MAX_COMPONENTS> &get_visuals()const{ ... ; }

    Input input;
    Camera camera;
    EntityManager entity_manager;

    SystemPlayer system_player;
    SystemEnemy system_enemy;
    SystemPhysics system_physics;
    SystemRenderStatic system_render_static;
};

#endif
