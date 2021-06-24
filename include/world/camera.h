#ifndef WORLD_CAMERA_H
#define WORLD_CAMERA_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

struct Camera {
    double aspect_ratio;
    glm::vec2 pos;
    double zoom;
    double orientation;
    std::size_t num_levels;
    static constexpr double base_scaling = 0.001f;

    mutable glm::mat4 view;
    void update_view_matrix() const {
        view = glm::scale(glm::vec3(base_scaling*zoom, base_scaling*zoom*aspect_ratio, 1.0f/num_levels))
            * glm::rotate((float)orientation, glm::vec3(0, 0, 1))
            * glm::translate(glm::vec3(-pos.x, -pos.y, 0));
    }

    // aspect ratio = width / height
    Camera(double aspect_ratio): aspect_ratio(aspect_ratio), pos(0, 0), zoom(1), orientation(0) {}
};

#endif
