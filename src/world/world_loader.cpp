#include "world/world_loader.h"
#include "world/world.h"

void load_world(World &world)
{
    world.clear();
    entities.clear();
    actions.clear();
    states.clear();
    visuals.clear();

    Action action;
    State state;
    Visual visual;

    // Create player

    action.type = ActionType::PLAYER;
    visual.render_index = 3;
    visual.type = VisualType::SPRITE;
    visual.depth = 4;
    create_entity(&action, &state, &visual);

    // Create moving enemy

    action.type = ActionType::RANDOM_WALK;
    state.pos.x = -100;
    visual.render_index = 0;
    visual.depth = 5;
    create_entity(&action, &state, &visual);

    // Create stationary entity

    state.pos.x = 200;
    state.pos.y = 100;
    visual.render_index = 4;
    visual.depth = 8;
    create_entity(nullptr, &state, &visual);

    // Create entity with polygon

    // action.type = ActionType::RANDOM_WALK;
    state.pos.x = -100;
    state.pos.y = 150;
    visual.render_index = 0;
    visual.type = VisualType::MESH;
    visual.depth = 10;
    create_entity(nullptr, &state, &visual);
}

