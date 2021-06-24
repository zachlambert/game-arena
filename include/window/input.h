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
    CLICK_RIGHT
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
    Input(const Window &window);

    void register_key(InputType input_id, int key)const;
    void register_mouse_button(InputType input_id, int button)const;
    bool input_down(InputType input_id)const;
    InputState query_input_state(InputType id)const;
    glm::vec2 get_mouse_pos()const;
private:
    const Window &window;
};

#endif
