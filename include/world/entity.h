#ifndef WORLD_ENTITY_H
#define WORLD_ENTITY_H

#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

enum class ActionType {
    PLAYER,
    RANDOM_WALK
};

struct Action {
    int entity_id;
    ActionType type;
};

struct State {
    int entity_id;
    glm::vec2 pos;
    double orientation;
    glm::vec3 twist;
    State(): entity_id(0), pos(0, 0), orientation(0), twist(0, 0, 0) {}
};

enum class VisualType {
    SPRITE,
    MESH
};

struct Visual {
    int entity_id;
    VisualType type;
    unsigned int render_index; // sprite_index, mesh_index, etc
    mutable glm::mat4 model;
    unsigned int depth;
    Visual(): depth(8) {}
};

struct Entity {
    int action_id;
    int state_id;
    int visual_id;
    Entity(): action_id(-1), state_id(-1), visual_id(-1) {}
};

#endif
