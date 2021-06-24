#ifndef WORLD_WORLD_H
#define WORLD_WORLD_H

#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

struct Camera {
    glm::vec2 pos;
    double zoom;
    double orientation;
    std::size_t num_levels;
    static constexpr double base_scaling = 0.001f;

    mutable glm::mat4 view;
    void update_view_matrix() const {
        view = glm::scale(glm::vec3(base_scaling*zoom, base_scaling*zoom, 1.0f/num_levels))
            * glm::rotate((float)orientation, glm::vec3(0, 0, 1))
            * glm::translate(glm::vec3(-pos.x, -pos.y, 0));
    }

    Camera(): pos(0, 0), zoom(1), orientation(0) {}
};

struct Entity {
    glm::vec2 pos;
    double depth;
    double orientation;

    mutable glm::mat4 model;
    // TODO: Only make this update when parameters have changed
    // OR have two types of entity: one which never or rarely moves, where
    // you can keep a flag for when anything changes.
    // For the other, except it to continually change so recalculate each time.
    void update_model_matrix() const {
        model = glm::translate(glm::vec3(pos.x, pos.y, depth))
                * glm::rotate((float)orientation, glm::vec3(0, 0, 1));
    }
    int sprite_index; // -1 For no sprite

    Entity(): pos(0, 0), depth(-1), orientation(0) {}
};

struct World {
    Camera camera;
    std::vector<Entity> entities;
};

#endif
