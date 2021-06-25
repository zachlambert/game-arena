#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <thread>
#include <chrono>

#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "window/window.h"
#include "window/input.h"
#include "window/clock.h"
#include "render/renderer.h"
#include "game/game.h"

int main()
{
    Window window("Arena", 1080, 720);
    if (!window.initialise()) {
        return 1;
    }

    Renderer renderer("data/");
    renderer.initialise();

    Game game(window);
    game.create_default_world();

    game.input.register_key(InputType::MOVE_RIGHT, GLFW_KEY_D);
    game.input.register_key(InputType::MOVE_LEFT, GLFW_KEY_A);
    game.input.register_key(InputType::MOVE_UP, GLFW_KEY_W);
    game.input.register_key(InputType::MOVE_DOWN, GLFW_KEY_S);
    game.input.register_mouse_button(InputType::CLICK_LEFT, GLFW_MOUSE_BUTTON_LEFT);

    Clock clock;

    while (window.is_running()) {
        window.update();
        game.update(clock.sample_dt());
        renderer.render(game);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
