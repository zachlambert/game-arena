#ifndef GAME_COMPONENTS_H
#define GAME_COMPONENTS_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

enum class ComponentType {
    TRANSFORM,
    PHYSICS,
    VISUAL_STATIC,
    // HUMAN,
    // ENEMY,
    GUN
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
    glm::vec2 scale;
    Transform(): pos(0, 0), orientation(0), scale(1, 1) {}
};

struct Physics: public BaseComponent {
    glm::vec3 twist;
    Physics(): twist(0, 0, 0) {}
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

// struct Human: public BaseComponent {
//     double move_speed;
//     double strafe_speed;
//     double turn_speed;
//     Human(): move_speed(0), strafe_speed(0), turn_speed(0) {}
// };
// 
// struct Enemy: public BaseComponent {
//     glm::vec2 goal_pos;
//     glm::vec2 point_target_pos;
//     Enemy(): goal_pos(0, 0), point_target_pos(0, 0) {}
// };

struct Gun: public BaseComponent {
    double origin_offset;
    double focus;
    double max_focus;
    double focus_rate;
    double base_damage;
    Gun(): origin_offset(150), focus(0), max_focus(1), focus_rate(0.5), base_damage(10) {}
};

} // namespace component

#endif
