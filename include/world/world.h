#ifndef WORLD_WORLD_H
#define WORLD_WORLD_H

#include <vector>

#include "world/camera.h"
#include "world/entity.h"
#include "window/input.h"

class World {
public:
    World(const Camera &camera): camera(camera) {}

    void clear();
    int entity_create();
    Action &entity_create_action(int id);
    unsigned int create_entity();
    void create_default_world();
    void create_entity(
        const Action *action_in,
        const State *state_in,
        const Visual *visual_in
    );

    void update(double dt, const Input &input);
    const Camera &get_camera()const{ return camera; }
    const std::vector<Visual> &get_visuals()const{ return visuals; }
private:
    std::vector<Entity> entities;

    std::vector<State> states;
    std::vector<Action> actions;

    Camera camera;
    std::vector<Visual> visuals;
};

#endif
