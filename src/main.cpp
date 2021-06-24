#include <stdlib.h>
#include <stdio.h>
#include <vector>

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
#include "world/world.h"

int main()
{
    Window window("Arena", 1080, 720);
    if (!window.initialise()) {
        return 1;
    }

    Renderer renderer("data/");
    renderer.initialise();

    Camera camera(1080.0f/720);
    World world(camera);
    world.create_default_world();

    Clock clock;
    Input input(window);
    
    input.register_input(InputType::MOVE_RIGHT, GLFW_KEY_D);
    input.register_input(InputType::MOVE_LEFT, GLFW_KEY_A);
    input.register_input(InputType::MOVE_UP, GLFW_KEY_W);
    input.register_input(InputType::MOVE_DOWN, GLFW_KEY_S);

    while (window.is_running()) {
        window.update();
        world.update(clock.sample_dt(), input);
        renderer.render(world);
    }

    return 0;
}
