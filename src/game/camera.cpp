#include "game/camera.h"

void Camera::update_view_matrix() const {
    view = glm::scale(glm::vec3(base_scaling*zoom, base_scaling*zoom*aspect_ratio, 1.0f/num_levels))
        * glm::rotate((float)orientation, glm::vec3(0, 0, 1))
        * glm::translate(glm::vec3(-pos.x, -pos.y, 0));
}

glm::vec2 Camera::project_point(const glm::vec2 &screen_pos)const
{
    glm::vec4 screen_coords(0.0f, 0.0f, 0.0f, 1.0f);
    screen_coords.x = 2 * (screen_pos.x - (double)screen_width/2) / screen_width;
    screen_coords.y = - 2 * (screen_pos.y - (double)screen_height/2) / screen_height;

    glm::mat4 view_inverse =
        glm::translate(glm::vec3(pos.x, pos.y, 0))
        * glm::rotate(-(float)orientation, glm::vec3(0, 0, 1))
        * glm::scale(glm::vec3(1.0f/(base_scaling*zoom), 1.0f/(base_scaling*zoom*aspect_ratio), 1));
    glm::vec4 result = view_inverse * screen_coords;

    return glm::vec2(result.x, result.y);
}

void Camera::adjust_zoom(double zoom_amount)
{
    zoom = std::exp(std::log(zoom) + zoom_rate*zoom_amount);
    if (zoom < zoom_lower_limit) zoom = zoom_lower_limit;
    if (zoom > zoom_upper_limit) zoom = zoom_upper_limit;
}
