#ifndef GAME_CAMERA_H
#define GAME_CAMERA_H

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
    void update_view_matrix()const;
    glm::vec2 project_point(const glm::vec2 &screen_pos)const;

    double zoom_rate;
    double zoom_lower_limit;
    double zoom_upper_limit;
    void adjust_zoom(double zoom_amount);

    // aspect ratio = screen_width / height
    Camera(int screen_width, int screen_height, std::size_t num_levels = 16, double zoom_rate=1):
        screen_width(screen_width),
        screen_height(screen_height),
        aspect_ratio((double)screen_width/screen_height),
        pos(0, 0), zoom(1), orientation(0),
        num_levels(num_levels),
        zoom_rate(zoom_rate),
        zoom_lower_limit(0.1),
        zoom_upper_limit(10) {}
};

#endif
