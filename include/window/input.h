#ifndef WINDOW_INPUT_H
#define WINDOW_INPUT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

enum class InputType {
    MOVE_RIGHT,
    MOVE_LEFT,
    MOVE_UP,
    MOVE_DOWN,
    CLICK_LEFT,
    CLICK_RIGHT,
    PAUSE,
    TOGGLE_GUN
};

enum class InputState {
    UP,
    PRESSED,
    DOWN,
    RELEASED,
    NONE
};

class Window;

class Input {
public:
    Input(const Window &window, double scroll_scale = 0.2);

    void register_key(InputType input_id, int key)const;
    void register_mouse_button(InputType input_id, int button)const;

    bool input_down(InputType input_id)const;
    InputState query_input_state(InputType id)const;
    glm::vec2 get_mouse_pos()const;

    double get_scroll_amount()const;
private:
    const Window &window;
    double scroll_scale;
};

#endif
