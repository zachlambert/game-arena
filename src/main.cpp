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
#include "window/clock.h"
#include "render/renderer.h"
#include "world/world.h"

int main()
{
    Window window("Arena", 1080, 720);
    if (!window.initialise()) {
        return 1;
    }

    Renderer renderer;
    renderer.initialise();

    Camera camera(1080.0f/720);
    World world(camera);
    create_default_world(world);

    Clock clock;

    while (window.is_running()) {
        world.update(clock.sample_dt());
        renderer.render(world);
        window.update();
    }

    return 0;
}
