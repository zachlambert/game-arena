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

#include "setup/resources.h"

#include "window/window.h"
#include "window/input.h"
#include "window/clock.h"
#include "window/fps_counter.h"
#include "render/renderer.h"
#include "game/game.h"

int main()
{
    // 1. Create a window

    // Window window("Arena", 1920, 1080, true);
    Window window("Arena", 1080, 720, false);
    if (!window.initialise()) {
        return 1;
    }

    // 2. Load resources

    Resources resources;
    load_spritesheets(resources.spritesheets);

    // 3. Initialise game state

    Game game(window);
    game.initialise(resources);

    game.input.register_key(InputType::MOVE_RIGHT, GLFW_KEY_D);
    game.input.register_key(InputType::MOVE_LEFT, GLFW_KEY_A);
    game.input.register_key(InputType::MOVE_UP, GLFW_KEY_W);
    game.input.register_key(InputType::MOVE_DOWN, GLFW_KEY_S);
    game.input.register_key(InputType::PAUSE, GLFW_KEY_P);
    game.input.register_key(InputType::TOGGLE_GUN, GLFW_KEY_E);
    game.input.register_mouse_button(InputType::CLICK_LEFT, GLFW_MOUSE_BUTTON_LEFT);

    // 4. Initialise renderer

    Renderer renderer("data/");
    renderer.initialise(resources, game);

    // 5. Start game loop

    Clock clock;
    FpsCounter fps_counter;

    while (window.is_running()) {
        window.update();
        game.update(clock.sample_dt());
        renderer.render(game);
        game.cleanup();
        fps_counter.update();
        // std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}
