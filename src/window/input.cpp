#include "window/input.h"
#include "window/window.h"

#include <unordered_map>
static std::unordered_map<InputType, InputState> inputs;
static std::unordered_map<int, InputType> key_mappings;
static std::unordered_map<int, InputType> mouse_button_mappings;
static double scroll_yoffset;

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

void mouse_button_callback(
    GLFWwindow* window,
    int button,
    int action,
    int mods)
{
    auto search = mouse_button_mappings.find(button);
    if (search != mouse_button_mappings.end()) {
        InputType input_id = search->second;
        if (action == GLFW_PRESS) {
            inputs[input_id] = InputState::PRESSED;
        } else if (action == GLFW_RELEASE) {
            inputs[input_id] = InputState::RELEASED;
        }
    }
}

void mouse_scroll_callback(
    GLFWwindow *window,
    double xoffset,
    double yoffset)
{
    scroll_yoffset += yoffset;
}

Input::Input(const Window &window, double scroll_scale):
    window(window), scroll_scale(scroll_scale)
{
    glfwSetKeyCallback(window.window, key_callback);
    glfwSetMouseButtonCallback(window.window, mouse_button_callback);
    glfwSetScrollCallback(window.window, mouse_scroll_callback);
    scroll_yoffset = 0;
}

void Input::register_key(InputType input_id, int key)const
{
    key_mappings[key] = input_id;
    inputs[input_id] = InputState::UP;
}

void Input::register_mouse_button(InputType input_id, int button)const
{
    mouse_button_mappings[button] = input_id;
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

double Input::get_scroll_amount()const
{
    double return_value = scroll_yoffset * scroll_scale;
    scroll_yoffset = 0;
    return return_value;
}
