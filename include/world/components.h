#ifndef WORLD_COMPONENTS_H
#define WORLD_COMPONENTS_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

namespace component {

struct Transform {
    glm::vec2 pos;
    double orientation;
};

struct Physics {
    glm::vec3 twist;
};

struct VisualStatic {
    enum class Type {
        SPRITE,
        MESH
    };
    glm::mat4 model;
    unsigned int render_index;
    Type type;
};

struct Human {
    double move_speed;
    double strafe_speed;
    double turn_speed;
};

struct Enemy {
    glm::vec2 goal_pos;
    glm::vec2 point_target_pos;
};

} // namespace component

#endif
