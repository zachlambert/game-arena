#ifndef WORLD_WORLD_H
#define WORLD_WORLD_H

#include <vector>

#include "world/camera.h"
#include "world/entity.h"
#include "window/input.h"

class World {
public:
    World(const Camera &camera): camera(camera) {}

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
    void update_action(int action_id, const Input &input);
    void update_state(int state_id, double dt);
    void update_visual(int visual_id);

    std::vector<Entity> entities;

    std::vector<State> states;
    std::vector<Action> actions;

    Camera camera;
    std::vector<Visual> visuals;
};

#endif
