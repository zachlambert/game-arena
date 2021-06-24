#ifndef WORLD_CAMERA_H
#define WORLD_CAMERA_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

struct Camera {
    int screen_width, screen_height;
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

    glm::vec2 project_point(const glm::vec2 &screen_pos)
    {
        glm::vec4 screen_coords(0.0f, 0.0f, 0.0f, 1.0f);
        screen_coords.x = 2 * (screen_pos.x - (double)screen_width/2) / screen_width;
        screen_coords.y = - 2 * (screen_pos.y - (double)screen_height/2) / screen_height;

        glm::mat4 view_inverse =
            glm::translate(glm::vec3(pos.x, pos.y, 0))
            * glm::rotate(-(float)orientation, glm::vec3(0, 0, 1))
            * glm::scale(glm::vec3(1.0f/(base_scaling*zoom), 1.0f/(base_scaling*zoom), 1));
        glm::vec4 result = view_inverse * screen_coords;

        return glm::vec2(result.x, result.y);
    }

    // aspect ratio = screen_width / height
    Camera(int screen_width, int screen_height):
        screen_width(screen_width),
        screen_height(screen_height),
        aspect_ratio((double)screen_width/screen_height),
        pos(0, 0), zoom(1), orientation(0) {}
};

#endif
