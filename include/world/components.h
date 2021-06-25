#ifndef WORLD_COMPONENTS_H
#define WORLD_COMPONENTS_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

enum class ComponentType {
    TRANSFORM,
    PHYSICS,
    VISUAL_STATIC,
    HUMAN,
    ENEMY
};

struct ComponentReference {
    ComponentType type;
    int index;
};

namespace component {

struct BaseComponent {
    // If a component is moved, need to update the index in the reference
    int ref_id;
};

struct Transform: public BaseComponent {
    glm::vec2 pos;
    double orientation;
};

struct Physics: public BaseComponent {
    glm::vec3 twist;
};

struct VisualStatic: public BaseComponent {
    enum class Type {
        SPRITE,
        MESH
    };
    Type type;
    glm::mat4 model;
    unsigned int render_index;
    unsigned int depth;
};

struct Human: public BaseComponent {
    double move_speed;
    double strafe_speed;
    double turn_speed;
};

struct Enemy: public BaseComponent {
    glm::vec2 goal_pos;
    glm::vec2 point_target_pos;
};

} // namespace component

#endif
