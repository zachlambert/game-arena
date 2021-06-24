#include "window/input.h"
#include "window/window.h"

#include <unordered_map>
std::unordered_map<InputType, InputState> inputs;
std::unordered_map<int, InputType> key_mappings;

void key_callback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods)
{
    auto search = key_mappings.find(key);
    if (search != key_mappings.end()) {
        InputType input_id = search->second;
        if (action == GLFW_PRESS) {
            inputs[input_id] = InputState::PRESSED;
        } else if (action == GLFW_RELEASE) {
            inputs[input_id] = InputState::RELEASED;
        }
    }
}

Input::Input(const Window &window): window(window)
{
    glfwSetKeyCallback(window.window, key_callback);
}

void Input::register_input(InputType input_id, int key)const
{
    key_mappings[key] = input_id;
    inputs[input_id] = InputState::UP;
}

bool Input::input_down(InputType input_id)const
{
    return inputs[input_id] == InputState::PRESSED
        || inputs[input_id] == InputState::DOWN;
}

InputState Input::query_input_state(InputType input_id)const
{
    auto search = inputs.find(input_id);
    if (search == inputs.end()) return InputState::NONE;

    InputState state = search->second;
    if (state == InputState::PRESSED) search->second = InputState::DOWN;
    if (state == InputState::RELEASED) search->second = InputState::UP;
    return state;
}

glm::vec2 Input::get_mouse_pos()const
{
    double mx, my;
    glfwGetCursorPos(window.window, &mx, &my);
    return glm::vec2(mx, my);
}
