#include "world/world.h"

#include <iostream>
#include <random>

void World::create_default_world()
{
    entities.clear();
    actions.clear();
    states.clear();
    visuals.clear();

    Action action;
    State state;
    Visual visual;

    // Create player

    action.type = ActionType::PLAYER;
    visual.sprite_index = 3;
    create_entity(&action, &state, &visual);

    // Create moving enemy

    action.type = ActionType::RANDOM_WALK;
    state.pos.x = -100;
    visual.sprite_index = 0;
    create_entity(&action, &state, &visual);

    // Create stationary entity

    state.pos.x = 200;
    state.pos.y = 100;
    visual.sprite_index = 4;
    create_entity(nullptr, &state, &visual);
}

void World::create_entity(
    const Action *action_in,
    const State *state_in,
    const Visual *visual_in)
{
    Entity entity;

    if (action_in != nullptr) {
        entity.action_id = states.size();
        Action action(*action_in);
        action.entity_id = entities.size();
        actions.push_back(action);
    } else {
        entity.action_id = -1;
    }

    if (state_in != nullptr) {
        entity.state_id = states.size();
        State state(*state_in);
        state.entity_id = entities.size();
        states.push_back(state);
    } else {
        entity.state_id = -1;
    }

    if (visual_in != nullptr) {
        entity.visual_id = states.size();
        Visual visual(*visual_in);
        visual.entity_id = entities.size();
        visuals.push_back(visual);
    } else {
        entity.visual_id = -1;
    }

    entities.push_back(entity);
}

void update_action_player(State &state, const Input &input, Camera &camera)
{
    state.twist.x = 0;
    if (input.input_down(InputType::MOVE_RIGHT)) {
        state.twist.x += 800;
    }
    if (input.input_down(InputType::MOVE_LEFT)) {
        state.twist.x -= 800;
    }

    state.twist.y = 0;
    if (input.input_down(InputType::MOVE_UP)) {
        state.twist.y += 800;
    }
    if (input.input_down(InputType::MOVE_DOWN)) {
        state.twist.y -= 800;
    }

    glm::vec2 mouse_pos_screen = input.get_mouse_pos();
    glm::vec2 mouse_pos_world = camera.project_point(mouse_pos_screen);
    state.orientation = std::atan2(mouse_pos_world.y - state.pos.y, mouse_pos_world.x - state.pos.x);

    if (input.query_input_state(InputType::CLICK_LEFT) == InputState::PRESSED)
    {
        std::cout << "MOUSE PRESSED" << std::endl;
    }
}

void update_action_random_walk(State &state)
{
    static std::default_random_engine generator;
    static std::normal_distribution<double> distribution(0, 1);
    state.twist.x = state.twist.x*0.99 + distribution(generator)*25;
    state.twist.y = state.twist.y*0.99 + distribution(generator)*25;
    state.twist.z = state.twist.z*0.99 + distribution(generator)*0.5;
}

void update_action(Action &action, State &state, const Input &input, Camera &camera)
{
    switch(action.type) {
        case ActionType::PLAYER:
            update_action_player(state, input, camera);
            break;
        case ActionType::RANDOM_WALK:
            update_action_random_walk(state);
            break;
    }
}

void update_state(State &state, double dt)
{
    state.pos.x += state.twist.x * dt;
    state.pos.y += state.twist.y * dt;
    state.orientation += state.twist.z * dt;
    if (state.orientation < -M_PI) state.orientation += 2*M_PI;
    if (state.orientation > M_PI) state.orientation -= 2*M_PI;
}

void update_visual(Visual &visual, const State &state)
{
    visual.model = glm::translate(glm::vec3(state.pos.x, state.pos.y, visual.depth))
        * glm::rotate((float)state.orientation, glm::vec3(0, 0, 1));
}

void World::update(double dt, const Input &input)
{
    for (std::size_t i = 0; i < actions.size(); i++) {
        Action &action = actions[i];
        State &state = states[entities[action.entity_id].state_id];
        update_action(action, state, input, camera);
    }

    for (std::size_t i = 0; i < states.size(); i++) {
        State &state = states[i];
        update_state(state, dt);

        // Also update camera pos
        int action_id = entities[state.entity_id].action_id;
        if (action_id >= 0 && actions[action_id].type == ActionType::PLAYER) {
            camera.pos = state.pos;
        }
    }

    for (std::size_t i = 0; i < visuals.size(); i++) {
        Visual &visual = visuals[i];
        State &state = states[entities[visual.entity_id].state_id];
        update_visual(visual, state);
    }
    camera.update_view_matrix();
}
